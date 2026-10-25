// [0.0.3 T9 T10] specs/features/0.0.3-windows.md
#pragma once
#include <kgc/secondPanel/CellArrangement.h>
#include <QObject>
namespace kgc::secondPanel {
/** 세컨드 패널의 배치 선택과 저장을 맡아요.
 * 입력: 배치 설정.
 * 출력: 선택한 배치와 칸 비율.
 * 실패: 없음.
 */
class SecondPanelService : public QObject {
    Q_OBJECT
public:
    /** 배치 설정 변경을 화면에 이어줘요.
     * 입력: 부모 객체.
     * 출력: 없음.
     * 실패: 없음.
     */
    explicit SecondPanelService(QObject* parent = nullptr);
    /** 저장된 배치를 읽어요.
     * 입력: 없음.
     * 출력: 배치 열거 값.
     * 실패: 없음.
     */
    settings::Arrangement arrangement() const;
    /** 선택한 배치의 칸 비율을 읽어요.
     * 입력: 없음.
     * 출력: 사각형 목록.
     * 실패: 없음.
     */
    QList<QRectF> cells() const;
    /** 새 배치를 저장해 화면에 바로 반영해요.
     * 입력: 배치 열거 값.
     * 출력: 없음.
     * 실패: 정의되지 않은 값은 설정 저장소가 거부해요.
     * 흐름:
     * 1. settings::setArrangement — 배치 값을 운영 설정에 저장해요.
     */
    void setArrangement(settings::Arrangement arrangement);
signals:
    /** 배치 설정 변경을 알려줘요.
     * 입력: 없음.
     * 출력: 변경 알림.
     * 실패: 없음.
     */
    void arrangementChanged();
};
}
