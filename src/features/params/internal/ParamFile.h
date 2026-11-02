// [0.3.0 5.5 6.8-1 T3] specs/formats/params.md
#pragma once
#include <kgc/app/errors/Result.h>
#include <kgc/params/Params.h>
#include <QByteArray>
#include <QList>
#include <QPair>
#include <QString>
namespace kgc::params {
/** 파일 머리에 적는 기체 정보예요.
 * 입력: sysid, 펌웨어 이름, MAV_TYPE 번호, 펌웨어 버전이에요.
 * 출력: writeParamFile이 머리 주석에 써요.
 * 실패: 없어요.
 */
struct ParamFileHeader { int sysid; QString firmwareName; int vehicleType; QString firmwareVersion; };
/** 읽은 파일의 내용이에요.
 * 입력: parseParamFile이 채워요.
 * 출력: 이름과 값 entries, 나쁜 줄 수 badLines, 다른 컴포넌트 줄 수 otherComponent예요.
 * 실패: 없어요.
 */
struct ParsedParamFile { QList<QPair<QString, double>> entries; int badLines = 0; int otherComponent = 0; };
/** 파라미터 파일 내용을 만들어요.
 * 입력: 머리 정보 header와 행 rows예요.
 * 출력: QGC 형식의 UTF-8 바이트이며 FC 값만 이름 자연순으로 담아요.
 * 실패: 없어요.
 */
QByteArray writeParamFile(const ParamFileHeader& header, const QList<Param>& rows);
/** 파라미터 파일 내용을 읽어요.
 * 입력: 파일 바이트 bytes예요. QGC 다섯 칸과 Mission Planner 두 칸을 읽어요.
 * 출력: 읽은 항목이에요. 같은 이름이 또 나오면 뒤 값을 써요.
 * 실패: 읽은 항목이 없으면 FileEmpty예요.
 */
kgc::app::errors::Result<ParsedParamFile> parseParamFile(const QByteArray& bytes);
}
