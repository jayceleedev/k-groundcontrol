// [0.3.0 6.3 T4] specs/features/0.3.0-params.md
#pragma once
#include <kgc/params/ParamMeta.h>
#include <QByteArray>
#include <QHash>
#include <QString>
namespace kgc::params {
/** PX4 목록형 메타데이터를 읽어요.
 * 입력: parameters.json 바이트 bytes예요.
 * 출력: 이름으로 찾는 메타데이터예요.
 * 실패: 깨진 JSON은 빈 해시예요.
 */
QHash<QString, ParamMeta> parseFlatMetadata(const QByteArray& bytes);
/** ArduPilot 묶음형 메타데이터를 읽어요.
 * 입력: apm.pdef.json 바이트 bytes예요. 최상위 키 json은 건너뛰어요.
 * 출력: 이름으로 찾는 메타데이터예요. 기본값은 없어요.
 * 실패: 깨진 JSON은 빈 해시예요.
 */
QHash<QString, ParamMeta> parseGroupedMetadata(const QByteArray& bytes);
}
