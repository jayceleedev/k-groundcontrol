// [0.3.0 6.8 6.8-1] specs/features/0.3.0-params.md
/** 표 아래 고정 영역이에요. 값 오류 글과 고른 행의 자세한 내용을 보여요.
 * 입력: ParamsController.selected와 오류 글 errorText예요.
 * 출력: 기본값으로·되돌리기 요청이에요.
 * 실패: 고른 행이 없고 오류도 없으면 숨어요.
 */
import QtQuick
import QtQuick.Layouts
import Kgc.Ui
import Kgc.Params

Rectangle {
    id: detail
    objectName: "paramsDetail"
    property string errorText: ""
    readonly property var info: ParamsController.selected
    readonly property bool hasRow: info.name !== undefined
    visible: hasRow || errorText !== ""
    implicitHeight: content.implicitHeight + Theme.inset * 2
    radius: Theme.radiusSmall
    color: Theme.surfaceAlt
    ColumnLayout {
        id: content
        anchors.fill: parent
        anchors.margins: Theme.inset
        spacing: Theme.choiceGap
        Text {
            objectName: "paramsEditError"
            Layout.fillWidth: true
            visible: detail.errorText !== ""
            text: detail.errorText
            color: Theme.danger
            font.family: Theme.fontFamily
            font.pixelSize: Theme.fontSize
            wrapMode: Text.Wrap
        }
        RowLayout {
            visible: detail.hasRow
            spacing: Theme.inset
            Text {
                text: detail.info.name ?? ""
                color: Theme.ink
                font.family: Theme.monoFamily
                font.pixelSize: Theme.fontSize
                font.bold: true
            }
            Text {
                text: detail.info.hasPending
                    ? qsTr("FC 값 %1 · 새 값 %2").arg(detail.info.fcText).arg(detail.info.pendingText)
                    : qsTr("FC 값 %1").arg(detail.info.fcText ?? "")
                color: Theme.inkMuted
                font.family: Theme.fontFamily
                font.pixelSize: Theme.fontSize
            }
        }
        Text {
            Layout.fillWidth: true
            visible: detail.hasRow && text !== ""
            text: detail.info.description ?? ""
            color: Theme.inkMuted
            font.family: Theme.fontFamily
            font.pixelSize: Theme.fontSize
            wrapMode: Text.Wrap
            maximumLineCount: 4
            elide: Text.ElideRight
        }
        RowLayout {
            visible: detail.hasRow && (detail.info.canRestore || detail.info.hasPending)
            spacing: Theme.inset
            ActionButton {
                objectName: "paramsRestoreDefault"
                visible: detail.info.canRestore ?? false
                kind: "secondary"
                text: qsTr("기본값으로")
                onClicked: ParamsController.restoreDefault(detail.info.name)
            }
            ActionButton {
                objectName: "paramsRevert"
                visible: detail.info.hasPending ?? false
                kind: "secondary"
                text: qsTr("되돌리기")
                onClicked: ParamsController.revert(detail.info.name)
            }
        }
    }
}
