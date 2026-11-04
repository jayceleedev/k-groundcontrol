// [0.4.1 5.4 T4] specs/features/0.4.1-board-match.md
// [0.2.0 T10] specs/features/0.2.0-fcflash.md
#pragma once
#include "BoardMatch.h"
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
/** px4-boards.json의 항목 하나예요.
 * 입력: px4Boards가 채워요.
 * 출력: 보드 ID boardId, 빌드 이름 앞부분 prefix, 보드 이름 맞추기에 쓰는 USB 이름 names(없으면 빈 목록)예요.
 * 실패: 없어요.
 */
struct Px4Board {
    int boardId = 0;
    QString prefix;
    QStringList names;
};
/** GitHub 릴리스 목록을 해석해요.
 * 입력: 응답 본문 json이에요.
 * 출력: 초안과 프리릴리스를 뺀 릴리스의 최신 순 목록이에요.
 * 실패: 읽을 수 없으면 빈 목록이에요.
 */
QList<Px4Release> parsePx4Releases(const QByteArray& json);
/** px4-boards.json의 항목 전부를 읽어요.
 * 입력: 파일 내용 boardsJson이에요.
 * 출력: 파일 순서대로 boardId, prefix, names예요.
 * 실패: 읽을 수 없으면 빈 목록이에요.
 */
QList<Px4Board> px4Boards(const QByteArray& boardsJson);
/** px4-boards.json에서 보드의 prefix를 찾아요.
 * 입력: 파일 내용 boardsJson과 보드 ID boardId예요. 보드 255는 9로 봐요.
 * 출력: px4Boards 중 boardId가 같은 prefix 전부예요(빈 prefix와 겹친 값은 빼요).
 * 실패: 읽을 수 없으면 빈 목록이에요.
 */
QStringList px4Prefixes(const QByteArray& boardsJson, int boardId);
/** 릴리스에서 보드에 맞는 파일을 골라요.
 * 입력: release와 prefixes예요.
 * 출력: 파일 <prefix>_<꼬리표>.px4 전부이며, 꼬리표 bootloader는 빼요.
 * 실패: 없어요.
 */
QList<Px4Candidate> px4Candidates(const Px4Release& release, const QStringList& prefixes);
/** 릴리스에서 보드 ID가 다른 보드의 파일을 행으로 만들어요.
 * 입력: release, px4Boards의 결과 boards, FC의 보드 ID boardId예요.
 * 출력: compatible하지 않은 보드의 prefix에 맞는 <prefix>_<꼬리표>.px4 파일마다 {"<prefix> · <꼬리표>", 그 보드 ID}예요.
 *       꼬리표 bootloader와 px4-boards.json에 없는 파일은 빼요. 이름을 대소문자 무시로, 같으면 보드 ID 작은 순으로 정렬해요.
 * 실패: 없어요.
 */
QList<OtherBoard> px4OtherBoards(const Px4Release& release, const QList<Px4Board>& boards, int boardId);
}
