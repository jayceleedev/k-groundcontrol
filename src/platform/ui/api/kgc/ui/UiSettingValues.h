// [0.0.2 T2–T9 T16 T20] specs/features/0.0.2-theme.md
#pragma once
#include <QObject>
namespace kgc::ui {
    /** 공유 화면 설정을 화면에 노출해요.
     * 입력: 없음.
     * 출력: 없음.
     * 실패: 없음.
     */
class UiSettingValues : public QObject {
    Q_OBJECT
    Q_PROPERTY(int holdToConfirmMs READ holdToConfirmMs WRITE setHoldToConfirmMs NOTIFY holdToConfirmMsChanged)
    Q_PROPERTY(int holdToConfirmMsMin READ holdToConfirmMsMin CONSTANT)
    Q_PROPERTY(int holdToConfirmMsMax READ holdToConfirmMsMax CONSTANT)
public:
    /** 설정 변경을 구독해요.
     * 입력: 부모 객체.
     * 출력: 없음.
     * 실패: 없음.
     */
    explicit UiSettingValues(QObject* parent = nullptr);
    /** 길게 누르는 시간을 읽어요.
     * 입력: 없음.
     * 출력: 밀리초.
     * 실패: 없음.
     */
    int holdToConfirmMs() const;
    /** 허용하는 최소 시간을 읽어요.
     * 입력: 없음.
     * 출력: 밀리초.
     * 실패: 없음.
     */
    int holdToConfirmMsMin() const;
    /** 허용하는 최대 시간을 읽어요.
     * 입력: 없음.
     * 출력: 밀리초.
     * 실패: 없음.
     */
    int holdToConfirmMsMax() const;
    /** 길게 누르는 시간을 저장해요.
     * 입력: 밀리초.
     * 출력: 없음. QML 속성 쓰기이므로 저장 결과를 반환하지 않아요.
     * 실패: 생성 접근자가 false로 거부한 범위 밖 값은 저장하지 않고 기존 값과 변경 시그널을 유지해요.
     * 흐름:
     * 1. settings::setHoldToConfirmMs — 메타데이터 범위를 지켜 시간을 저장해요.
     */
    void setHoldToConfirmMs(int value);
signals:
    /** 시간 설정의 변경을 알려요.
     * 입력: 없음.
     * 출력: 없음.
     * 실패: 없음.
     */
    void holdToConfirmMsChanged();
};
}
