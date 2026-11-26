// [0.5.0 5.6.3 T1] specs/features/0.5.0-map.md
#pragma once
#include <QByteArray>
#include <QString>
namespace kgc::map {
/** 타일 바이트가 어떤 그림 형식인지 앞 바이트로 알아봐요.
 * 입력: 받은 바이트 bytes예요.
 * 출력: FF D8 FF로 시작하면 "jpeg", 89 50 4E 47 0D 0A 1A 0A로 시작하면 "png", 그 밖은 빈 문자열이에요.
 * 실패: 없어요.
 */
QString tileImageFormat(const QByteArray& bytes);
}
