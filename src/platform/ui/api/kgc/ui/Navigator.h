// [0.0.1 T1] specs/features/0.0.1-shell.md
#pragma once
#include <QObject>

namespace kgc::ui {
/** 패널을 이름으로 열도록 요청해요.
 * 입력: 없음.
 * 출력: 없음.
 * 실패: 없음.
 */
class Navigator : public QObject {
    Q_OBJECT
public:
/** 탐색 요청 통로를 만들어요.
 * 입력: 부모.
 * 출력: 없음.
 * 실패: 없음.
 */
    explicit Navigator(QObject* parent = nullptr);
/** 패널 열기 요청을 전달해요.
 * 입력: 패널 식별자와 인자.
 * 출력: 없음.
 * 실패: 없음.
 * 흐름:
 * 1. openRequested — 창에 패널 열기 요청을 알려요.
 */
    Q_INVOKABLE void open(const QString& panelId, const QString& arg = {});
signals:
/** 패널을 여는 쪽에 요청을 알려요.
 * 입력: 패널 식별자와 인자.
 * 출력: 없음.
 * 실패: 없음.
 */
    void openRequested(const QString& panelId, const QString& arg);
};
}
