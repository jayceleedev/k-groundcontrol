# 규격: 설정 메타데이터 `*.settings.json` (BASE §4)

- 상태: 초안 1 — 2026-10-24, 0.0.1 스펙과 함께
- 자리: 각 모듈 `internal/settings/<id>.settings.json`. 읽는 코드는 `platform/app/settings`와 생성기 `cmake/gen/SettingsGen.cmake`뿐.

## 파일

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
    },
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
| `settings[].id` | `<모듈>.<이름>`. 모듈 접두사는 `module`과 같아야 함. 이름은 lowerCamelCase | 예 |
| `type` | `bool` · `int` · `double` · `string` · `enum` · `list<object>` | 예 |
| `default` | 기본값(형에 맞게). 기본값은 **여기에만** 둔다 | 예 |
| `min`, `max` | `int`·`double`만 | 아니오 |
| `enum` | `type`이 `enum`일 때 값 문자열 목록 | enum이면 예 |
| `item` | `type`이 `list<object>`일 때 항목의 필드 정의(같은 표 재귀) | list면 예 |
| `labelKey` | 번역 문맥 "Settings"의 한국어 원문(화면 라벨·검색어) | 예 |
| `category` | 앱 설정 화면의 묶음 = 모듈 id | 예 |
| `restart` | `true`면 바꾼 뒤 재시작 필요 표시 | 아니오(기본 false) |

## 생성되는 것

`${CMAKE_BINARY_DIR}/gen/kgc/<module>/Settings.h`:

```cpp
namespace kgc::shell::settings {
  bool leftMenuCollapsed();                 // 현재 값
  void setLeftMenuCollapsed(bool);          // 쓰기(저장 + changed 시그널)
  inline constexpr const char* kLeftMenuCollapsed = "shell.leftMenuCollapsed"; // 내부용 키(생성기만 씀)
}
```
- `enum`은 `enum class Language { System, Ko, En }`과 변환 함수가 생긴다.
- 코드에서 키 문자열을 직접 쓰지 않는다(검사 §14 "설정 키").

## 저장
QSettings IniFormat, 조직·앱 이름 `K-GroundControl`. 키는 `id` 그대로(점을 `/`로 바꿔 그룹화: `[shell] leftMenuCollapsed=false`).
