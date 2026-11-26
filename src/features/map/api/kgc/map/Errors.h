// [0.5.0 5.6.2 6] specs/features/0.5.0-map.md
#pragma once
#include <kgc/map/ModuleId.h>
#include <QString>
namespace kgc::map {
/** 지도 영역 받기와 타일 저장이 실패한 까닭을 나타내요.
 * 입력: 스펙 §5.6의 실패 상황이에요.
 * 출력: 각 상황에 대응하는 MapError 값이에요.
 * 실패: 없어요.
 */
enum class MapError : std::uint16_t {
    TooManyTiles = 1, BadZoomRange, DownloadBusy, TileFailed, StoreFailed
};
/** 지도 오류를 공통 코드로 바꿔요.
 * 입력: 변환할 MapError인 error예요.
 * 출력: map 모듈 번호와 오류 번호를 담은 ErrorCode예요.
 * 실패: 없어요.
 */
inline kgc::app::errors::ErrorCode make(MapError error)
{
    return {kModuleId, static_cast<std::uint16_t>(error)};
}
/** 지도 오류를 화면 문구로 바꿔요.
 * 입력: 표시할 공통 오류 코드 error예요.
 * 출력: 현재 언어의 문구(번역 문맥 "Map", 스펙 §6)이며, 다른 모듈의 코드는 빈 문자열이에요.
 * 실패: 없어요.
 */
QString errorText(kgc::app::errors::ErrorCode error);
}
