# 규격: 설정 메타데이터 `*.settings.json` (BASE §4)

- 상태: 초안 2 — 2026-10-24, 0.0.1 스펙과 함께(검토 2 N7 반영: 예시를 모듈별 두 파일로)
- 자리: 각 모듈 `internal/settings/<id>.settings.json`. 읽는 코드는 `platform/app/settings`와 생성기 `cmake/gen/SettingsGen.cmake`뿐.

## 파일 (모듈마다 하나)

`src/features/shell/internal/settings/shell.settings.json`
```json
{
  "module": "shell",
  "settings": [
    {
      "id": "shell.leftMenuCollapsed",
      "type": "bool",
      "default": false,
      "labelKey": "좌 메뉴 접힘",
      "category": "shell"
    }
  ]
}
```

`src/features/appSettings/internal/settings/appSettings.settings.json`
```json
{
  "module": "appSettings",
  "settings": [
    {
      "id": "appSettings.language",
      "type": "enum",
      "enum": ["system", "ko", "en"],
      "default": "system",
      "labelKey": "언어",
      "category": "appSettings"
    }
  ]
}
```

| 키 | 뜻 | 필수 |
|---|---|---|
| `module` | 모듈 id. 파일 이름 `<module>.settings.json`과 같아야 함 | 예 |
| `settings[].id` | `<모듈>.<이름>`. 모듈 접두사는 `module`과 같아야 함(생성기가 검사해 다르면 configure 오류). 이름은 lowerCamelCase | 예 |
| `type` | `bool` · `int` · `double` · `string` · `enum` · `list<object>` | 예 |
| `default` | 기본값(형에 맞게). 기본값은 **여기에만** 둔다 | 예 |
| `min`, `max` | `int`·`double`만 | 아니오 |
| `enum` | `type`이 `enum`일 때 값 문자열 목록 | enum이면 예 |
| `item` | `type`이 `list<object>`일 때 항목의 필드 정의(같은 표 재귀) | list면 예 |
| `labelKey` | 번역 문맥 "Settings"의 한국어 원문(화면 라벨·검색어) | 예 |
| `category` | 앱 설정 화면의 묶음 = 모듈 id | 예 |
| `restart` | `true`면 바꾼 뒤 재시작 필요 표시 | 아니오(기본 false) |

## 생성되는 것

`${CMAKE_BINARY_DIR}/gen/<module>/kgc/<module>/Settings.h` (include는 `<kgc/<module>/Settings.h>`; 모듈 타깃은 `kgc_use_settings(<target> <module>)`로 include 경로와 `kgc_app_settings` 링크를 받음; enum은 `<name>Values()`, min/max가 있으면 `<name>Min()`·`<name>Max()`도 생성; `Settings`는 범위 밖 쓰기를 거부하고 범위 밖 저장값은 default로 읽음):

```cpp
namespace kgc::shell::settings {
  bool leftMenuCollapsed();                 // 현재 값
  void setLeftMenuCollapsed(bool);          // 쓰기(저장 + Settings::changed(id) 시그널)
}
```
- `enum`은 `enum class Language { System, Ko, En }`과 `toString(Language)`/`fromString(QStringView)`이 함께 생긴다.
- 설정마다 변경 구독 함수가 생긴다: `QMetaObject::Connection onLanguageChanged(QObject* receiver, std::function<void(Language)>)` — 안에서 `Settings::changed(id)`를 받아 키를 견준다(키 문자열은 생성 코드 안에만).
- 키 문자열은 생성 코드 안에만 있다. 손으로 쓴 코드에 설정 키 문자열을 쓰지 않는다(검사 §14 "설정 키").
- 생성기는 configure 때 모든 `*.settings.json`을 `platform/app/settings`의 등록 테이블(`${CMAKE_BINARY_DIR}/gen/kgc/app/settings/Registry.cpp`)에도 모아 넣는다. 그래서 `app/settings`는 모듈을 모르면서 전체 목록(기본값·형·라벨)을 안다.

## 저장
QSettings IniFormat, `QSettings(IniFormat, UserScope, "K-GroundControl", "K-GroundControl")`. 키는 `id`의 점을 `/`로 바꿔 그룹화: `[shell] leftMenuCollapsed=false`. Linux에서는 `XDG_CONFIG_HOME`을 따르므로 시험은 그 변수로 격리한다.
