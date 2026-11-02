// [0.3.0 6.8 6.8-1] specs/features/0.3.0-params.md
/** 파라미터 표와 묶음, 단추, 배너와 토스트를 한 화면에 모아요.
 * 입력: ParamsController의 표시값이에요.
 * 출력: 값 편집과 쓰기·파일·재부팅 요청이에요.
 * 실패: 컨트롤러의 toast와 배너로 보여요.
 * 흐름:
 * 1. requestWrite — 편집 중인 값을 확정하고 ParamsController.writeCheck로 쓸 수 있는지 봐요.
 * 2. WriteConfirmDialog.accepted → ParamsController.write — 값을 써요.
 * 3. FileDialog.accepted → ParamsController.openFile·saveFile — 파일을 비교하거나 저장해요.
 * 4. FileCompareDialog.accepted → ParamsController.applyFile — 고른 값을 대기값으로 올려요.
 */
import QtQuick
import QtQuick.Layouts
import QtQuick.Dialogs
import QtQuick.Controls.Basic as Basic
import Kgc.Ui
import Kgc.Params

Item {
    id: panel
    objectName: "paramsPanel"
    property Item glassSource: null
    /** 메뉴에서 열었을 때 초점을 받아요. 입력: 탐색 인자예요. 출력: 초점이에요. 실패: 없어요. */
    function onArg(arg: var) { forceActiveFocus() }
    /** 편집 중인 값을 확정한 뒤 쓰기 확인 창을 열어요. 입력: 없어요. 출력: 창이 열려요. 실패: 시동 중이면 토스트가 떠요. */
    function requestWrite() {
        table.commitEditor()
        if (table.editError !== "") {
            table.focusEditor()
            return
        }
        if (table.dialogOpen || ParamsController.pendingCount === 0) return
        const decision = ParamsController.writeCheck()
        if (decision === "deny") return
        writeDialog.rows = ParamsController.pendingRows()
        writeDialog.hold = decision === "warn"
        writeDialog.open()
    }
    // 닫기 단추가 visible을 끄므로, 조건이 다시 바뀔 때마다 새로 보이게 해요.
    component NoticeBanner: Banner {
        property bool shown: false
        Layout.fillWidth: true
        visible: false
        onShownChanged: visible = shown
        Component.onCompleted: visible = shown
    }
    EmptyState {
        objectName: "paramsEmpty"
        anchors.fill: parent
        visible: !ParamsController.hasVehicle
        title: qsTr("기체를 연결하면 파라미터가 보여요")
    }
    ColumnLayout {
        anchors.fill: parent
        visible: ParamsController.hasVehicle
        spacing: Theme.inset
        JobView {
            objectName: "paramsJob"
            Layout.fillWidth: true
            visible: ParamsController.job !== null
            job: ParamsController.job
            showError: false
        }
        NoticeBanner {
            objectName: "paramsIncomplete"
            level: "warn"
            shown: ParamsController.downloadState === ParamStates.Incomplete
            text: ParamsController.totalCount > 0
                ? qsTr("FC가 파라미터를 다 보내지 않았어요 (%1/%2)").arg(ParamsController.receivedCount).arg(ParamsController.totalCount)
                : qsTr("FC가 파라미터를 다 보내지 않았어요")
        }
        NoticeBanner {
            objectName: "paramsSkipped"
            level: "info"
            shown: ParamsController.downloadState === ParamStates.SkippedArmed
            text: qsTr("시동 중이라 파라미터를 받지 않았어요. 새로고침을 누르면 받아요.")
        }
        NoticeBanner {
            objectName: "paramsReboot"
            level: "warn"
            shown: ParamsController.rebootNeeded
            text: qsTr("재부팅해야 적용되는 값이 있어요")
        }
        RowLayout {
            Layout.fillWidth: true
            spacing: Theme.inset
            SearchBox {
                objectName: "paramsSearch"
                Layout.fillWidth: true
                placeholderText: qsTr("이름이나 설명 검색")
                text: ParamsController.searchText
                onTextChanged: ParamsController.searchText = text
            }
            Pill {
                objectName: "paramsPendingOnly"
                text: qsTr("바꾼 값만 (%1)").arg(ParamsController.pendingCount)
                checked: ParamsController.pendingOnly
                onClicked: ParamsController.pendingOnly = checked
            }
            Pill {
                objectName: "paramsNonDefault"
                visible: ParamsController.hasDefaults
                text: qsTr("기본값과 다른 값")
                checked: ParamsController.nonDefaultOnly
                onClicked: ParamsController.nonDefaultOnly = checked
            }
            Text {
                objectName: "paramsMetadataFailed"
                visible: ParamsController.metadataState === ParamStates.Failed
                text: qsTr("설명을 받지 못했어요")
                color: Theme.inkMuted
                font.family: Theme.fontFamily
                font.pixelSize: Theme.captionSize
            }
            Text {
                objectName: "paramsVisibleCount"
                text: qsTr("%1개").arg(ParamsController.visibleCount)
                color: Theme.inkMuted
                font.family: Theme.fontFamily
                font.pixelSize: Theme.fontSize
            }
        }
        RowLayout {
            Layout.fillWidth: true
            Layout.fillHeight: true
            spacing: Theme.inset
            ParamGroups {
                Layout.preferredWidth: Theme.paramGroupWidth
                Layout.fillHeight: true
            }
            ColumnLayout {
                Layout.fillWidth: true
                Layout.fillHeight: true
                spacing: Theme.inset
                ParamTable {
                    id: table
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                }
                ParamDetail {
                    Layout.fillWidth: true
                    errorText: table.editError
                }
            }
            ParamActions {
                Layout.fillWidth: false
                Layout.preferredWidth: Theme.paramActionsWidth
                Layout.fillHeight: true
                onWriteRequested: panel.requestWrite()
                onOpenRequested: openDialog.open()
                onSaveRequested: {
                    saveDialog.currentFolder = ParamsController.saveFolder
                    saveDialog.selectedFile = ParamsController.saveFileUrl
                    saveDialog.open()
                }
                onRebootRequested: rebootDialog.open()
            }
        }
    }
    Shortcut {
        sequence: "Ctrl+S"
        enabled: panel.visible && ParamsController.hasVehicle && ParamsController.job === null
        onActivated: panel.requestWrite()
    }
    WriteConfirmDialog {
        id: writeDialog
        parent: Basic.Overlay.overlay
        anchors.centerIn: parent
        // 창이 열린 사이에 시동이 걸릴 수 있어서 확인을 누른 때 다시 판정해요.
        onAccepted: {
            const decision = ParamsController.writeCheck()
            if (decision === "deny") return
            if (decision === "warn" && !hold) {
                hold = true
                Qt.callLater(open)
                return
            }
            ParamsController.write()
        }
    }
    FileCompareDialog {
        id: compareDialog
        parent: Basic.Overlay.overlay
        anchors.centerIn: parent
    }
    ConfirmDialog {
        id: rebootDialog
        parent: Basic.Overlay.overlay
        anchors.centerIn: parent
        title: qsTr("FC를 재부팅할까요?")
        onAccepted: ParamsController.reboot()
    }
    FileDialog {
        id: openDialog
        title: qsTr("파라미터 파일 불러오기")
        nameFilters: [qsTr("파라미터 파일 (*.params *.param *.parm)")]
        onAccepted: if (ParamsController.openFile(selectedFile)) compareDialog.show()
    }
    FileDialog {
        id: saveDialog
        title: qsTr("파라미터 파일로 저장")
        fileMode: FileDialog.SaveFile
        defaultSuffix: "params"
        nameFilters: [qsTr("파라미터 파일 (*.params)")]
        onAccepted: ParamsController.saveFile(selectedFile)
    }
    Toast {
        id: toast
        objectName: "paramsToast"
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.bottom: parent.bottom
        anchors.bottomMargin: Theme.panelInset
        width: Math.min(implicitWidth, parent.width - Theme.panelInset * 2)
        wrapMode: Text.Wrap
        visible: false
        z: 1
    }
    TimerBox {
        id: toastTimer
        interval: Theme.paramToastMs
        onTriggered: toast.visible = false
    }
    Connections {
        target: ParamsController
        function onToast(text: string) {
            toast.text = text
            toast.visible = true
            toastTimer.restart()
        }
    }
}
