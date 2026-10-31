// [0.2.0 T10] specs/features/0.2.0-fcflash.md
#pragma once
#include <QByteArray>
#include <QList>
#include <QString>
#include <QStringList>
namespace kgc::fcFlash {
/** 릴리스에 붙은 파일 하나예요.
 * 입력: parsePx4Releases가 채워요.
 * 출력: 파일 이름 name과 내려받을 url이에요.
 * 실패: 없어요.
 */
struct Px4Asset {
    QString name;
    QString url;
};
/** 정식 릴리스 하나예요.
 * 입력: parsePx4Releases가 채워요.
 * 출력: 태그 tag, 태그의 숫자 버전 version(기체 값과 비교하는 값), 파일 assets예요.
 * 실패: 없어요.
 */
struct Px4Release {
    QString tag;
    QString version;
    QList<Px4Asset> assets;
};
/** 이 보드에 올릴 수 있는 파일 하나예요.
 * 입력: px4Candidates가 채워요.
 * 출력: prefix, 꼬리표 label, 내려받을 url이에요. 화면 표시는 <prefix> · <꼬리표>예요.
 * 실패: 없어요.
 */
struct Px4Candidate {
    QString prefix;
    QString label;
    QString url;
};
/** GitHub 릴리스 목록을 해석해요.
 * 입력: 응답 본문 json이에요.
 * 출력: 초안과 프리릴리스를 뺀 릴리스의 최신 순 목록이에요.
 * 실패: 읽을 수 없으면 빈 목록이에요.
 */
QList<Px4Release> parsePx4Releases(const QByteArray& json);
/** px4-boards.json에서 보드의 prefix를 찾아요.
 * 입력: 파일 내용 boardsJson과 보드 ID boardId예요.
 * 출력: boardId가 같은 prefix 전부예요.
 * 실패: 읽을 수 없으면 빈 목록이에요.
 */
QStringList px4Prefixes(const QByteArray& boardsJson, int boardId);
/** 릴리스에서 보드에 맞는 파일을 골라요.
 * 입력: release와 prefixes예요.
 * 출력: 파일 <prefix>_<꼬리표>.px4 전부이며, 꼬리표 bootloader는 빼요.
 * 실패: 없어요.
 */
QList<Px4Candidate> px4Candidates(const Px4Release& release, const QStringList& prefixes);
}
