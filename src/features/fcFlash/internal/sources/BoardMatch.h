// [0.4.1 5.3 5.4 T1 T2] specs/features/0.4.1-board-match.md
#pragma once
#include <kgc/fcFlash/FlashStates.h>
#include <QList>
#include <QString>
#include <QStringList>
namespace kgc::fcFlash {
/** 보드 이름 맞추기의 결과예요.
 * 입력: recognize가 채워요.
 * 출력: 종류 kind와, Matched일 때 맞은 후보의 위치 matched(후보 목록 순서)예요.
 * 실패: 없어요.
 */
struct MatchResult {
    BoardMatch kind = BoardMatch::NoName;
    QList<int> matched;
};
/** 화면에 낼 보드 ID가 다른 보드 한 행이에요.
 * 입력: manifestOtherBoards·px4OtherBoards가 채워요.
 * 출력: 표시 이름 name과 그 빌드의 보드 ID boardId예요.
 * 실패: 없어요.
 */
struct OtherBoard {
    QString name;
    int boardId = 0;
};
/** USB 보드 이름이 후보의 이름 중 하나와 같은지 봐요.
 * 입력: USB 장치가 알린 이름 usbName과 맞춰 볼 이름 목록 names예요.
 * 출력: 둘 다 앞뒤 공백을 빼고 대소문자를 무시해 같은 값이 하나라도 있으면 참이에요. 부분 일치는 거짓이에요.
 * 실패: usbName이 비면 거짓이에요.
 */
bool nameMatches(const QString& usbName, const QStringList& names);
/** USB 보드 이름을 같은 보드 ID 후보들과 맞춰 봐요.
 * 입력: usbName과, 후보마다 맞춰 볼 이름 목록 candidateNames(후보 순서)예요.
 * 출력: usbName이 비면 NoName, 하나 이상 맞으면 Matched와 맞은 위치들, 아무것도 안 맞으면 NoMatch예요.
 * 실패: 없어요.
 */
MatchResult recognize(const QString& usbName, const QList<QStringList>& candidateNames);
/** 처음에 골라 둘 후보 위치를 정해요.
 * 입력: recognize의 결과 result와 후보 수 count예요.
 * 출력: Matched이고 맞은 후보가 하나면 그 위치, NoName이면 0.2.0 규칙대로 pickCandidate(count)가 Auto일 때 0,
 *       그 밖은 -1(선택 없음)이에요. 보드 ID 255 검사는 부르는 쪽이 따로 해요.
 * 실패: 없어요.
 */
int preselect(const MatchResult& result, int count);
}
