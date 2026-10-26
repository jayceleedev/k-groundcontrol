// [0.0.4 T1] specs/features/0.0.4-righttabs.md
#pragma once
#include <QObject>
namespace kgc::logs {
/** 로그 모듈의 전역 부분(뷰어)이에요. 이번 버전엔 우 사이드바 로그 탭을 등록하는 자리만 맡아 상태와 함수가 없어요.
 * 입력: 부모 객체.
 * 출력: 상태를 갖지 않는 전역 객체.
 * 실패: 없음.
 */
class LogsViewer final : public QObject {
    Q_OBJECT
public:
    /** 부모와 함께 지워지는 뷰어를 만들어요.
     * 입력: 부모 객체.
     * 출력: 뷰어 인스턴스.
     * 실패: 없음.
     */
    explicit LogsViewer(QObject* parent = nullptr);
};
}
