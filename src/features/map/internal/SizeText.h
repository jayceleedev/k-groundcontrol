// [0.5.0 5.6.3 T3] specs/features/0.5.0-map.md
#pragma once
#include <QString>
#include <QtGlobal>
namespace kgc::map {
/** 바이트 수를 사람이 읽는 크기 글자로 바꿔요.
 * 입력: 바이트 수 bytes예요.
 * 출력: 1024 단위로 1024² 미만은 KB, 1024³ 미만은 MB, 그 밖은 GB(단위는 반올림 전에 골라요)이고,
 *       수는 소수 한 자리에서 끝의 ".0"을 뺀 "%1 %2" 글자예요(15 KB, 27.7 MB, 1.2 GB). 단위는 번역하지 않아요.
 * 실패: 없어요.
 */
QString sizeText(qint64 bytes);
}
