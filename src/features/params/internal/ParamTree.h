// [0.4.2 5.2 T1 T2 T3 T4 T5] specs/features/0.4.2-param-tree.md
#pragma once
#include <QList>
#include <QSet>
#include <QString>
#include <QStringList>
namespace kgc::params {
/** 파라미터 트리의 마디 하나예요.
 * 입력: buildParamTree가 채워요.
 * 출력: key는 가지면 접두어 + "_*"(예 "ATC_RAT_*"), 잎이면 파라미터 이름이에요. branch는 가지면 참,
 *       depth는 첫 단계가 0이고, count는 가지면 골랐을 때 표에 남는 파라미터 수, 잎이면 1이에요.
 * 실패: 없어요.
 */
struct ParamNode {
    QString key;
    bool branch = false;
    int depth = 0;
    int count = 0;
};
/** 파라미터 이름의 "_" 앞머리(접두어)로 여러 단계 트리를 만들어요.
 * 입력: Params가 드는 순서 그대로의 이름 목록 names예요(자연순, 중복 없음). 함수는 정렬하지 않아요.
 * 출력: 부모 바로 뒤에 자식들이 첫 이름 순서로 오는 앞선 순회 목록이에요. P + "_"로 시작하는 이름이
 *       둘 이상인 접두어 P만 가지가 되고, 숫자를 떼거나 자식 하나뿐인 단계를 줄이지 않아요.
 * 실패: 없어요. 빈 목록은 빈 결과예요.
 */
QList<ParamNode> buildParamTree(const QStringList& names);
/** 마디를 골랐을 때 그 이름이 표에 남는지 판정해요.
 * 입력: 파라미터 이름 name과 마디 key예요. 대소문자를 구분해요.
 * 출력: key가 "*"로 끝나면 끝의 "*"를 뗀 pattern으로 name이 시작하거나 name + "_"가 pattern과 같을 때 참이에요.
 *       "*"는 모든 이름이 참이에요. 그 밖의 key는 name과 같을 때만 참이에요.
 * 실패: 없어요.
 */
bool underParamNode(const QString& name, const QString& key);
/** 펼친 가지 아래의 마디만 골라요.
 * 입력: buildParamTree의 결과 tree와 펼친 가지의 key 집합 expanded예요. tree에 없는 key는 무시해요.
 * 출력: tree의 순서를 지키며 조상 가지가 모두 펼쳐진 마디만 담은 목록이에요. 첫 단계 마디는 언제나 들어가요.
 * 실패: 없어요.
 */
QList<ParamNode> visibleParamNodes(const QList<ParamNode>& tree, const QSet<QString>& expanded);
/** 마디 key를 화면에 쓸 글자로 바꿔요.
 * 입력: 마디 key예요.
 * 출력: 가지면 끝의 "_*"를 뗀 접두어("ATC_RAT_*" → "ATC_RAT"), 잎과 "*"는 key 그대로예요.
 * 실패: 없어요.
 */
QString paramNodeText(const QString& key);
}
