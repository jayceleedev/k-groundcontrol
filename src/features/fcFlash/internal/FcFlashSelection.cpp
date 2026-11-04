// [0.4.1 5.3 5.5 6.4] specs/features/0.4.1-board-match.md
// [0.2.0 T19 T23 T24] specs/features/0.2.0-fcflash.md
#include "FcFlashServicePrivate.h"
#include "Constants.h"
#include "Events.h"
#include "sources/Candidates.h"
#include <kgc/app/log/Log.h>
#include <QFile>
#include <QCoreApplication>
#include <algorithm>
namespace kgc::fcFlash {
void FcFlashService::chooseSource(FirmwareSource source)
{
    if (d_->phase != Phase::Ready || d_->step != Step::Firmware || d_->closing) return;
    d_->clearSelection();
    d_->source = source;
    d_->error = {};
    d_->image.reset();
    d_->file.clear();
    d_->expected.clear();
    d_->firmwareLabel.clear();
    d_->downloadUrl.clear();
    d_->version = -1;
    d_->candidate = -1;
    d_->versionNames.clear();
    d_->candidateNames.clear();
    d_->match = {};
    d_->otherBoardList.clear();
    d_->listError = false;
    d_->firmware = source == FirmwareSource::Px4Official ? "PX4" : source == FirmwareSource::ArduPilotOfficial ? "ArduPilot" : "";
    if (source != FirmwareSource::LocalFile) d_->fetchList();
    emit changed();
}
void FcFlashService::Private::fetchList()
{
    if (source == FirmwareSource::Px4Official && !releases.isEmpty()) { rebuildVersions(); return; }
    if (source == FirmwareSource::ArduPilotOfficial && manifests.contains(board.boardId)) { rebuildVersions(); return; }
    loading = true;
    const auto generation = selectionGeneration;
    const auto id = board.boardId;
    if (source == FirmwareSource::ArduPilotOfficial && !manifestGzip.isEmpty()) {
        QMetaObject::invokeMethod(worker, [this, bytes = manifestGzip, id, generation] {
            worker->parseManifest(bytes, id, generation);
        }, Qt::QueuedConnection);
        return;
    }
    const auto requested = source;
    listRequest = http.get(QUrl(requested == FirmwareSource::Px4Official ? kPx4ReleasesUrl : kArduPilotManifestUrl));
    QObject::connect(listRequest, &kgc::app::net::HttpRequest::finished, q, [this, requested, id, generation](auto result) {
        if (generation != selectionGeneration) return;
        listRequest.clear();
        if (!result) { loading = false; listError = true; emit q->changed(); return; }
        if (requested == FirmwareSource::ArduPilotOfficial) manifestGzip = *result;
        QMetaObject::invokeMethod(worker, [this, bytes = std::move(*result), requested, id, generation] {
            if (requested == FirmwareSource::Px4Official) worker->parseReleases(bytes, generation);
            else worker->parseManifest(bytes, id, generation);
        }, Qt::QueuedConnection);
    });
}
void FcFlashService::Private::rebuildVersions()
{
    versionNames.clear();
    if (source == FirmwareSource::Px4Official) {
        for (const auto& release : releases) versionNames.append(release.tag);
    } else {
        manifestVersionList = manifestVersions(manifests.value(board.boardId).entries, vehicleKind);
        for (const auto& item : manifestVersionList) versionNames.append(item.version);
    }
    version = versionNames.isEmpty() ? -1 : 0;
    rebuildCandidates();
}
void FcFlashService::Private::rebuildCandidates()
{
    candidateNames.clear();
    candidate = -1;
    clearSelection();
    QList<QStringList> names;
    QList<OtherBoard> others;
    if (version < 0) { updateMatch(names, others); emit q->changed(); return; }
    if (source == FirmwareSource::Px4Official) {
        QFile file(kPx4BoardsResource);
        if (!file.open(QIODevice::ReadOnly)) { listError = true; updateMatch(names, others); emit q->changed(); return; }
        const auto json = file.readAll();
        const auto boards = px4Boards(json);
        px4CandidateList = px4Candidates(releases.at(version), px4Prefixes(json, board.boardId));
        std::sort(px4CandidateList.begin(), px4CandidateList.end(), [](const auto& a, const auto& b) {
            return a.prefix + " · " + a.label < b.prefix + " · " + b.label;
        });
        for (const auto& item : px4CandidateList) {
            candidateNames.append(item.prefix + " · " + item.label);
            const auto found = std::find_if(boards.cbegin(), boards.cend(), [&item](const auto& value) { return value.prefix == item.prefix; });
            names.append(found == boards.cend() ? QStringList() : found->names);
        }
        others = px4OtherBoards(releases.at(version), boards, board.boardId);
    } else {
        const auto scan = manifests.value(board.boardId);
        const auto buildType = manifestVersionList.at(version).buildType;
        manifestCandidateList = manifestCandidates(scan.entries, vehicleKind, buildType);
        std::sort(manifestCandidateList.begin(), manifestCandidateList.end(), [](const auto& a, const auto& b) {
            return a.platform < b.platform;
        });
        for (const auto& item : manifestCandidateList) {
            candidateNames.append(item.platform);
            names.append(QStringList{item.platform} + item.bootloaderNames);
        }
        others = manifestOtherBoards(scan, vehicleKind, buildType);
    }
    updateMatch(names, others);
    // 보드 255(fmu-v3)는 0.2.0처럼 PX4에서 미리 고르지 않아요. 이름이 하나 맞아도 마찬가지예요.
    const auto pick = preselect(match, candidateNames.size());
    if (pick >= 0 && !(source == FirmwareSource::Px4Official && board.boardId == kBoardFmuV3)) q->chooseCandidate(pick);
    emit q->changed();
}
void FcFlashService::Private::updateMatch(const QList<QStringList>& names, const QList<OtherBoard>& others)
{
    match = recognize(usb.usbName, names);
    otherBoardList.clear();
    for (const auto& item : others) otherBoardList.append(QVariantMap{{"name", item.name}, {"boardId", item.boardId}});
    if (matchLogged && loggedMatch.kind == match.kind && loggedMatch.matched.size() == match.matched.size() && loggedName == usb.usbName) return;
    matchLogged = true;
    loggedMatch = match;
    loggedName = usb.usbName;
    const QString kind = match.kind == BoardMatch::Matched ? QStringLiteral("Matched")
        : match.kind == BoardMatch::NoMatch ? QStringLiteral("NoMatch") : QStringLiteral("NoName");
    KGC_LOG_INFO(kCategory, FcFlashEvent::BoardMatched, "Board match: %1, usb name: %2, matched: %3", kind, usb.usbName, match.matched.size());
}
void FcFlashService::chooseVehicle(const QString& name)
{
    if (d_->source != FirmwareSource::ArduPilotOfficial || d_->phase != Phase::Ready || d_->step != Step::Firmware) return;
    const QStringList names{"Copter", "Heli", "Plane", "Rover", "Sub"};
    const auto index = names.indexOf(name);
    if (index < 0) return;
    d_->clearSelection();
    d_->vehicleName = name;
    d_->vehicleKind = static_cast<ArduPilotVehicle>(index);
    d_->fetchList();
    emit changed();
}
void FcFlashService::chooseVersion(int index)
{
    if (d_->phase != Phase::Ready || d_->step != Step::Firmware || index < 0 || index >= d_->versionNames.size()) return;
    d_->version = index;
    d_->error = {};
    d_->rebuildCandidates();
}
void FcFlashService::chooseCandidate(int index)
{
    if (d_->phase != Phase::Ready || d_->step != Step::Firmware || index < 0 || index >= d_->candidateNames.size()) return;
    d_->clearSelection();
    d_->candidate = index;
    d_->firmwareLabel = d_->candidateNames.at(index);
    if (d_->source == FirmwareSource::Px4Official) {
        d_->expected = d_->releases.at(d_->version).version;
        d_->downloadUrl = d_->px4CandidateList.at(index).url;
    } else {
        d_->expected = d_->manifestCandidateList.at(index).version;
        d_->downloadUrl = d_->manifestCandidateList.at(index).url;
    }
    d_->error = {};
    emit changed();
}
void FcFlashService::chooseFile(const QString& path)
{
    if (d_->source != FirmwareSource::LocalFile || d_->phase != Phase::Ready || d_->step != Step::Firmware || d_->closing || path.isEmpty()) return;
    d_->clearSelection();
    d_->firmware.clear();
    d_->file = path;
    d_->error = {};
    d_->image.reset();
    d_->imageBusy = true;
    KGC_LOG_INFO(kCategory, FcFlashEvent::FileChosen, "Firmware file selected: %1", path);
    QMetaObject::invokeMethod(d_->worker, [this, path, generation = d_->selectionGeneration] {
        d_->worker->parseImage(path, generation);
    }, Qt::QueuedConnection);
    emit changed();
}
void FcFlashService::Private::imageReady(kgc::app::errors::Result<FirmwareImage> result)
{
    imageBusy = false;
    downloadPath.clear();
    if (phase != Phase::Ready && phase != Phase::Flashing) return;
    if (result) {
        fileBoardId = result->boardId;
        const auto checked = checkImage(*result, board.boardId, board.flashSize, board.extfSize);
        if (!checked) result = std::unexpected(checked.error());
    }
    if (!result) {
        error = result.error();
        if (phase == Phase::Flashing) flashFinished(std::unexpected(error));
        emit q->changed();
        return;
    }
    image = std::move(*result);
    firmware = image->kind == FirmwareKind::Px4 ? "PX4" : "ArduPilot";
    if (source == FirmwareSource::LocalFile) { expected = image->expectedVersion; firmwareLabel = image->summary; }
    if (phase == Phase::Flashing) runFlash();
    emit q->changed();
}
void FcFlashService::toConfirm()
{
    if (!canContinue()) return;
    d_->step = Step::Confirm;
    emit changed();
}
void FcFlashService::backToFirmware()
{
    if (d_->phase != Phase::Ready || d_->closing || d_->imageBusy) return;
    d_->step = Step::Firmware;
    emit changed();
}
}
