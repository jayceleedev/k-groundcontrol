// [0.0.4 T41 T42 T43] specs/features/0.0.4-righttabs.md
#pragma once
#include <QObject>
namespace kgc::reference {
/** 참고 모듈의 전역 수명을 맡아요.
 * 입력: 부모 객체.
 * 출력: 상태를 갖지 않는 서비스.
 * 실패: 없음.
 */
class ReferenceService final : public QObject {
    Q_OBJECT
public:
    /** 부모와 함께 지워지는 서비스를 만들어요.
     * 입력: 부모 객체.
     * 출력: 서비스 인스턴스.
     * 실패: 없음.
     */
    explicit ReferenceService(QObject* parent = nullptr);
};
}
