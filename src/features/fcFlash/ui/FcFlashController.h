// [0.2.0 T14–T23] specs/features/0.2.0-fcflash.md
#pragma once
#include <kgc/fcFlash/FcFlashService.h>
#include <QPointer>
#include <QQmlEngine>
namespace kgc::fcFlash {
/** 굽기 흐름의 값을 QML에 옮기고 서비스를 불러요.
 * 입력: 전역 서비스예요.
 * 출력: 서비스 값을 그대로 낸 표시값이에요. 판정은 서비스가 맡아요.
 * 실패: 없어요.
 */
class FcFlashController : public QObject {
    Q_OBJECT
    QML_ELEMENT
    QML_SINGLETON
    Q_PROPERTY(kgc::fcFlash::Step step READ step NOTIFY changed)
    Q_PROPERTY(kgc::fcFlash::Phase phase READ phase NOTIFY changed)
public:
/** 서비스를 부모로 삼아 화면 수명을 묶어요.
 * 입력: 서비스예요.
 * 출력: 서비스가 소유하는 컨트롤러예요.
 * 실패: 중복 등록은 프로그래밍 오류예요.
 */
    explicit FcFlashController(FcFlashService& service);
/** 등록된 객체를 QML 엔진에 제공해요.
 * 입력: QML 엔진과 스크립트 엔진이에요.
 * 출력: C++가 소유하는 싱글턴 포인터예요.
 * 실패: 등록 전 호출이나 다른 스레드의 엔진은 프로그래밍 오류예요.
 */
    static FcFlashController* create(QQmlEngine* engine, QJSEngine* scriptEngine);
    /** 입력: 없어요. 출력: 서비스의 단계예요. 실패: 없어요. */
    Step step() const { return service_.step(); }
    /** 입력: 없어요. 출력: 서비스의 Phase예요. 실패: 없어요. */
    Phase phase() const { return service_.phase(); }
/** 실패 글자를 현재 언어로 읽어요.
 * 입력: 없어요.
 * 출력: 서비스의 실패 글자이며, 없으면 빈 값이에요.
 * 실패: 없어요.
 */
    Q_INVOKABLE QString failureText() const { return service_.failureText(); }
    // WORKER-FILL: spec §6.9 — FcFlashService의 나머지 프로퍼티와 공개 함수를 Q_PROPERTY·Q_INVOKABLE로 옮기고 흐름을 적어요.
signals:
/** 표시값을 다시 읽도록 알려요.
 * 입력: 없어요.
 * 출력: 변경 알림이에요.
 * 실패: 없어요.
 */
    void changed();
private:
    FcFlashService& service_;
    inline static QPointer<FcFlashController> instance_;
};
}
