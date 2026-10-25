// [0.0.2 T8 T10–T14 T17 T22 T23] specs/features/0.0.2-theme.md
/** 같은 화면 위치의 지도만 흐리게 비추는 바탕이에요.
 * 입력: source에 지도 바닥을 넘기고 radius와 solidColor로 모양을 정해요.
 * 출력: 화면과 해당 부품의 변경 시그널이에요.
 * 실패: 잘못된 입력은 표시하지 않거나 범위 안으로 제한해요.
 */
import QtQuick
import QtQuick.Controls.Basic as Basic
import Kgc.Ui
import QtQuick.Effects

Rectangle {
    id: surface
    property Item source: null
    property color solidColor: Theme.surface
    readonly property point sourcePosition: {
        // 스크롤과 조상 배치가 바뀌어도 지도와 같은 위치를 비추도록 좌표를 구독해요.
        let ancestor = surface
        let position = Qt.point(0, 0)
        while (ancestor) { position = Qt.point(position.x + ancestor.x, position.y + ancestor.y); ancestor = ancestor.parent }
        return source ? mapToItem(source, 0, 0) : position
    }
    color: Theme.glass ? "transparent" : solidColor
    clip: true
    ShaderEffectSource {
        id: capture
        visible: false
        sourceItem: Theme.glass ? surface.source : null
        sourceRect: Qt.rect(surface.sourcePosition.x - Theme.glassBlurMax, surface.sourcePosition.y - Theme.glassBlurMax,
                            surface.width + Theme.glassBlurMax * 2, surface.height + Theme.glassBlurMax * 2)
        textureSize: Qt.size(surface.width + Theme.glassBlurMax * 2, surface.height + Theme.glassBlurMax * 2)
        hideSource: false
        live: true
    }
    MultiEffect {
        visible: Theme.glass && surface.source !== null
        x: -Theme.glassBlurMax; y: -Theme.glassBlurMax
        width: surface.width + Theme.glassBlurMax * 2
        height: surface.height + Theme.glassBlurMax * 2
        source: capture
        blurEnabled: true
        blur: Theme.glassBlur
        blurMax: Theme.glassBlurMax
        autoPaddingEnabled: false
    }
    Rectangle { anchors.fill: parent; radius: surface.radius; visible: Theme.glass; color: Theme.glassTint }
    Rectangle {
        anchors.fill: parent
        radius: surface.radius
        visible: Theme.glass
        opacity: Theme.glassShine
        gradient: Gradient {
            GradientStop { position: 0; color: Theme.glassEdge }
            GradientStop { position: 0.6; color: "transparent" }
        }
    }
    Rectangle { anchors.fill: parent; radius: surface.radius; color: "transparent"; border.color: Theme.glass ? Theme.glassEdge : Theme.line; border.width: Theme.lineWidth }
}
