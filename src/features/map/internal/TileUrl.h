// [0.5.0 5.6.3 T1] specs/features/0.5.0-map.md
#pragma once
#include <kgc/map/Settings.h>
#include <QString>
namespace kgc::map {
/** 타일 한 장을 받을 주소를 만들어요.
 * 입력: 지도 종류 type, 지명 언어 language("ko"·"en"), 확대 z와 타일 x·y, 시험 서버 host(parseTileHost를 거친 값이나 빈 값)예요.
 * 출력: host가 비면 https://mt{(x + y) mod 4}.google.com, 아니면 host 뒤에 /vt/lyrs={s|y|m}&hl={language}&x=&y=&z=를 붙인 주소예요.
 *       위성도 hl을 붙여요.
 * 실패: 없어요.
 */
QString tileUrl(settings::MapType type, const QString& language, int z, int x, int y, const QString& host);
/** 타일 저장소의 열쇠를 만들어요.
 * 입력: 지도 종류 type, 지명 언어 language, 확대 z와 타일 x·y예요.
 * 출력: 위성은 s/{z}/{x}/{y}, 하이브리드는 y/{language}/{z}/{x}/{y}, 일반은 m/{language}/{z}/{x}/{y}예요(위성은 라벨이 없어 언어를 넣지 않아요).
 * 실패: 없어요.
 */
QString tileKey(settings::MapType type, const QString& language, int z, int x, int y);
}
