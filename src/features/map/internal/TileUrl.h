// [0.5.1 5.3 T2] specs/features/0.5.1-map-sharp.md
// [0.5.0 5.6.3 T1] specs/features/0.5.0-map.md
#pragma once
#include <kgc/map/Settings.h>
#include <QString>
namespace kgc::map {
/** 타일 한 장을 받을 주소를 만들어요.
 * 입력: 지도 종류 type, 지명 언어 language("ko"·"en"), 타일 화소 배수 ratio(1 또는 2), 확대 z와 타일 x·y,
 *       시험 서버 host(parseTileHost를 거친 값이나 빈 값)예요.
 * 출력: host가 비면 https://mt{(x + y) mod 4}.google.com, 아니면 host 뒤에 /vt/lyrs={s|y|m}&hl={language}&x=&y=&z=를 붙인 주소예요.
 *       위성도 hl을 붙여요. ratio가 2일 때만 끝에 &scale=2(512 px 그림)를 붙이고, 1의 주소는 0.5.0과 한 글자도 다르지 않아요.
 * 실패: 1·2가 아닌 ratio는 프로그래밍 오류예요(Q_ASSERT).
 */
QString tileUrl(settings::MapType type, const QString& language, int ratio, int z, int x, int y, const QString& host);
/** 타일 저장소의 열쇠를 만들어요.
 * 입력: 지도 종류 type, 지명 언어 language, 타일 화소 배수 ratio(1 또는 2), 확대 z와 타일 x·y예요.
 * 출력: 위성은 s/{z}/{x}/{y}, 하이브리드는 y/{language}/{z}/{x}/{y}, 일반은 m/{language}/{z}/{x}/{y}예요(위성은 라벨이 없어 언어를 넣지 않아요).
 *       ratio가 2일 때만 끝에 @2를 붙여요. 1의 열쇠는 0.5.0과 같아 이미 저장된 타일과 받은 영역을 그대로 써요.
 * 실패: 1·2가 아닌 ratio는 프로그래밍 오류예요(Q_ASSERT).
 */
QString tileKey(settings::MapType type, const QString& language, int ratio, int z, int x, int y);
}
