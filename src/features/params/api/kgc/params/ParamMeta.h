// [0.3.0 5.5] specs/features/0.3.0-params.md
#pragma once
#include <QList>
#include <QString>
#include <optional>
namespace kgc::params {
/** 값 하나의 이름이에요.
 * 입력: 값 value와 이름 label이에요.
 * 출력: 선택지 목록의 한 줄이에요.
 * 실패: 없어요.
 */
struct ParamOption { double value; QString label; };
/** 비트 하나의 이름이에요.
 * 입력: 비트 번호 bit와 이름 label이에요.
 * 출력: 비트 목록의 한 줄이에요.
 * 실패: 없어요.
 */
struct ParamBit { int bit; QString label; };
/** 파라미터 하나의 설명 정보예요.
 * 입력: 내려받은 메타데이터를 해석한 값이에요.
 * 출력: 이름·제목·설명·단위·묶음·범위·기본값·선택지·비트예요.
 * 실패: 없는 값은 비어 있어요.
 */
struct ParamMeta {
    QString name, title, description, units, group;
    std::optional<double> min, max, defaultValue;
    bool rebootRequired = false, readOnly = false;
    QList<ParamOption> options;
    QList<ParamBit> bits;
};
}
