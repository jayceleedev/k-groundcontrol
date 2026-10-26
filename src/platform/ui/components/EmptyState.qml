// [0.0.4 T5a T5b T9b] specs/features/0.0.4-righttabs.md
/** 보여 줄 내용이 아직 없을 때 점선 상자 가운데에 까닭과 안내를 보여줘요.
 * 입력: title(제목 한 줄)과 detail(설명, 비면 설명 줄 없음)을 받아요.
 * 출력: 둘 다 inkMuted로, 제목은 fontSize, 설명은 captionSize로 폭에 맞춰 줄바꿈해 그려요. 테두리는 line 색 점선이에요.
 *       기본 높이는 내용 높이 + 위아래 Theme.inset이고, 부르는 쪽이 Layout.fillHeight로 남은 칸을 채우게 할 수 있어요.
 * 실패: 누를 것이 없어 입력을 받지 않아요.
 */
import QtQuick
import QtQuick.Shapes
import Kgc.Ui

Item {
    id: emptyState
    property string title
    property string detail
    implicitWidth: Theme.keyValueWidth
    implicitHeight: column.implicitHeight + Theme.inset * 2
    // Rectangle의 테두리는 점선을 못 그려서 Shape로 상자 테두리를 그려요.
    Shape {
        anchors.fill: parent
        preferredRendererType: Shape.CurveRenderer
        ShapePath {
            fillColor: "transparent"
            strokeColor: Theme.line
            strokeWidth: Theme.lineWidth
            strokeStyle: ShapePath.DashLine
            PathRectangle {
                x: Theme.lineWidth / 2
                y: Theme.lineWidth / 2
                width: emptyState.width - Theme.lineWidth
                height: emptyState.height - Theme.lineWidth
            }
        }
    }
    Column {
        id: column
        anchors.centerIn: parent
        width: parent.width - Theme.inset * 2
        spacing: Theme.valueLabelGap
        Text {
            width: parent.width
            text: emptyState.title
            color: Theme.inkMuted
            font.family: Theme.fontFamily
            font.pixelSize: Theme.fontSize
            font.bold: true
            horizontalAlignment: Text.AlignHCenter
            wrapMode: Text.Wrap
        }
        Text {
            width: parent.width
            visible: text.length > 0
            text: emptyState.detail
            color: Theme.inkMuted
            font.family: Theme.fontFamily
            font.pixelSize: Theme.captionSize
            horizontalAlignment: Text.AlignHCenter
            wrapMode: Text.Wrap
        }
    }
}
