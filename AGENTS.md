# AGENTS.md — rules for implementers of K-GroundControl

You are implementing K-GroundControl, a GPL-3.0 ground control station for PX4 and ArduPilot, in C++23 / Qt 6.12 for Windows and Linux. The PM (Claude) writes specs in Korean; you implement from the brief you are given. The user reads only what the PM writes.

**Authority.** `specs/BASE.md` (Korean) is the foundation and the original; this file is its English translation for implementers. If they disagree, BASE wins and you report the discrepancy. Feature behaviour comes from the feature spec named in your brief (`specs/features/*.md`). Never implement anything the brief and spec do not ask for.

**Read in this order before writing code:** this file → `specs/BASE.md` → the feature spec in your brief → the `MODULE.md` of every module you will touch.

## 1. Structure — modular monolith

One executable. Inside, modules with hard boundaries that talk only through public interfaces.

| Kind | Where | May depend on |
|---|---|---|
| Platform modules (know nothing about features) | `src/platform/app` (0) < `comm` (1) < `vehicle` (2) < `ui` (2.5) | lower platform layers only |
| Feature modules (one feature = one module, logic + screen) | `src/features/<id>/` | all platform modules + `api/` of features **ranked below** in BASE §3.2 |
| Composition | `src/main/` | everything; no logic |

Every module is one CMake static library target. Only `api/` is PUBLIC; `internal/` and `ui/` are PRIVATE.

```
src/features/esc/
  api/kgc/esc/      EscService.h EscStates.h Errors.h ModuleId.h Settings.h(generated)
  internal/         logic, Constants.h, Events.h, settings/esc.settings.json, paths/<transport>/
  ui/               EscPanel.qml, EscController.{h,cpp}, register.cpp
  CMakeLists.txt
  MODULE.md
```

Rules that the compiler and `tools/check` enforce — do not work around them:
- Include public headers as `<kgc/<module path>/X.h>` (`<kgc/comm/mavlink/Mavlink.h>`, `<kgc/esc/EscService.h>`). Never `#include "../.."`. Never include another module's `internal/` or `ui/`. `api/` headers never include `internal/` (use pimpl or interface + factory).
- Feature rank table (BASE §3.2): 1 shell · 2 notify ai appSettings analyze link · 3 fly mission vehicleInfo preflight logs · 4 layout · 5 sensors radio flightModes power failsafe motors tuning fcFlash esc elrs tracker · 6 map · 7 video terrain gcsposition · 8 params · 9 extras. A module may use the `api/` of any module with a **larger** rank number. Same rank: never. To reach "upward": emit a signal, publish state, register a contribution (slot/panel/layer), or call `Navigator::open(id, arg)`.
- Inside a feature: `ui/` → `internal/` → `api/`, one direction.
- Namespaces follow the module path: `kgc::app`, `kgc::comm::mavlink`, `kgc::vehicle::firmware`, `kgc::esc`.
- Size limits (physical lines): file 400, function 60, QML file 300. Split instead of exceeding.
- `MODULE.md` in every module (template in BASE §12, ≤100 lines). Update it in the same commit whenever the module's `api/` or its dependencies change.
- Platform code never names a feature. Exception: the `OperationKind` enum in `app/job` lists feature-named operations as labels only.

## 2. Where things live (BASE §4–§11)

| Thing | Place | Rule |
|---|---|---|
| Boot options | `platform/app/boot/BootOptions` (CLI + `kgc.ini`) | only place that reads env vars |
| Operating settings | `internal/settings/<id>.settings.json` in the owning module; stored via `platform/app/settings/Settings` (QSettings) | metadata first; use the generated typed accessors `kgc::<id>::settings::name()`; never a key string; defaults only in JSON |
| Constants | module `internal/Constants.h`; shared ones in `platform/app/constants/` | no magic timing/count/address/magic literals elsewhere (check scope in BASE §6) |
| Secrets | `platform/app/secrets/SecretStore` (QtKeychain), type `Secret` | `reveal()` only in allowed files; never log |
| Vehicle state | `platform/vehicle/states/VehicleStates` bundles; owner `VehicleStateOwner`; feature-owned bundles exposed by the feature's per-vehicle service | read-only for everyone else; never cache vehicle values; large lists are `QAbstractListModel` |
| GCS-side arm blockers | `vehicle/safety/ArmGuard::block(reason)` → token, `release(token)` | |
| Sending to the vehicle | `vehicle/outbound/VehicleSender` is the only door; `vehicle/command/CommandQueue` adds retries | never pack `command_long/int` outside `vehicle/command`; never call link write APIs outside `comm/` and `vehicle/outbound/` |
| Armed-state policy | declare an `OperationKind`, call `vehicle/safety/ArmedPolicy::check(kind, scope)` before starting work | table in BASE §9.2; `VehicleScope::Any` for vehicle-less flashing |
| Port ownership | `comm/rawport/PortRegistry`; raw bytes via `ByteStream`/`RawPort`; flashing/passthrough via `comm/lease/PortLease::acquire(port, quietFor)` | never create `QSerialPort` outside `comm/links/serial` and `comm/rawport` |
| Long-running work | `platform/app/job/Job` (state, step, progress, ETA, cancel, `finished(Result<void>)`) | hold leases/locks through `job.hold(Releaser)` |
| Errors | `Result<T> = std::expected<T, ErrorCode>`; `ErrorCode = {ModuleId, code}`; module enums in `api/kgc/<id>/Errors.h` | no exceptions for expected failures |
| Logging | `KGC_LOG(kCategory, <Module>Event::X, "메시지", values…)` only | never `qDebug/qInfo/qWarning` or QML `console.log`; events enumerated in `internal/Events.h` |
| Notifications | `platform/app/notify/Notify` (level, source, time) | the only entrance for bell, toast, TTS |
| Periodic work | `platform/app/scheduler/Scheduler::register(name, period, owner, context)`; one-shot timeouts via `Timeout` | no `QTimer`, `singleShot`, `startTimer`, `QBasicTimer`, QML `Timer` elsewhere (QML `Timer` only inside `platform/ui/components/TimerBox`) |
| HTTP | `platform/app/net/Http` (HTTPS only, cache + hash) | no `QNetworkAccessManager` elsewhere |
| SQL | `platform/app/store` | no QtSql elsewhere |
| Geo math | `platform/app/geo/Altitude` (takes `AltitudeFrame`), `Geodesy` | one place |
| Firmware differences | `platform/vehicle/firmware/` plugins return **data and pure functions only** (capabilities, mode table, `CommandStep` lists, metadata sources, layout mapping, deny lists) | never compare `MAV_AUTOPILOT_*` or the strings "PX4"/"ArduPilot"/"APM" outside that folder (allow-list in BASE §8) |
| Transport-specific procedures | feature `internal/paths/<transport>/` (e.g. `esc/internal/paths/blhPassthrough`) | folder names are transports, never firmware names |
| Screen | `ui/<Id>Panel.qml` + `<Id>Controller` (Q_PROPERTY/Q_INVOKABLE); `ui/register.cpp` registers panels, slot contributions, map layers via `platform/ui` | controllers only map state to display values and call services; no decisions |
| QML imports | `QtQuick*`, `QtQml*`, the Qt modules allowed in BASE §2, `Kgc.Ui`, own `Kgc.<Id>`, and `Kgc.<Id>` of lower-ranked features | nothing else |
| MAVLink headers | only via `<kgc/comm/mavlink/Mavlink.h>`; allowed in `comm`, `vehicle`, feature `internal/` | never in `app`, `platform/ui`, or any `ui/` |

If the feature spec's "기반" section does not list a value, state, function or place you need: **stop and report**. Do not invent a new place.

## 3. Language

| Where | Language |
|---|---|
| Code comments, docstrings, test descriptions | Korean, explanatory and friendly ("…해요." style), written so a non-expert can follow. Identifiers stay English inside them |
| Log messages (`KGC_LOG` text), terminal output, API error messages | Korean, telegraphic noun-ending phrases: values after a colon, clauses separated by commas ("링크 끊김: COM7, 3초 뒤 재접속"). No sentence endings (…다/…요) |
| Strings shown on screen | Korean source text wrapped in `tr()` / `qsTr()`; English lives in `i18n/kgc_en.ts` |
| Identifiers, enum names, log event names, error codes, commit subjects, this file, your reports | English |

## 4. Comment rules

- Write why, not what. Do not restate the code.
- Every public class and function (everything in `api/`) has a docstring: first line its role in one sentence, then `입력` / `출력` / `실패` in that order.
- An entry point the user follows (a QML action handler → controller method → service operation) also has a `흐름` section after `실패`: numbered steps in the order the code runs them, one per line, naming the call and what it does in plain Korean. The controller's 흐름 names the service operation it hands over to; the service's 흐름 lists the platform calls. Keep it in step with the code in the same commit.
- Code that comes from a spec starts with its source tag: `// [0.0.1 T4] specs/features/0.0.1-shell.md`. Shared rules use the document and section: `// [BASE 9.2] specs/BASE.md`.
- A stub to be filled is marked `// WORKER-FILL: <what to implement>`; whoever fills it removes the marker.
- Never: commented-out code, a TODO without an owner, change history (dates, names), sentences mixing Korean and English.

## 5. Working rules

- Implement exactly the acceptance items (T-numbers) of the spec in your brief, in the modules the spec names. Nothing else.
- Before every commit run `python3 tools/check/run.py` and the `tests/` target; both must pass. Do not add exceptions to the checks.
- Commit messages: English subject starting with the spec id (`0.0.1: add Shell window frame (T2, T3)`), body in English. Commit small, per acceptance item or per module.
- Do not read or write anything under `.pm/` except the brief file you were given and the report file it names.
- Do not change `specs/` (the PM owns it). If a spec is ambiguous or wrong, write the question in your report and implement the unambiguous parts.
- Third-party code only via CPM with pinned tag and hash; never vendor files by hand. Qt via aqtinstall in CI.
- Never run flashing, passthrough or motor commands against real hardware unless the brief says the device is on the bench and props are off.

## 6. Report back (English, short)

1. Commit range you produced.
2. For each acceptance item: done / not done / blocked, with the file and the test or check that proves it.
3. Questions about the spec (quote the line).
4. Anything you could not run (toolchain, display, hardware) and why.
