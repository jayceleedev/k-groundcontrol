// [0.2.0 T19 T23 T24] specs/features/0.2.0-fcflash.md
/** 확인한 보드에 맞는 공식 펌웨어나 파일을 골라요.
 * 입력: 보드 정보, 목록과 파일 해석 결과예요.
 * 출력: 출처·버전·후보 선택과 다음 단계 요청이에요.
 * 실패: 목록 오류와 파일의 호환 오류를 보여요.
 * 흐름:
 * 1. FcFlashController.chooseSource·chooseVehicle·chooseVersion·chooseCandidate·chooseFile — 선택을 서비스에 넘겨요.
 * 2. FcFlashController.toConfirm·bootAndReset — 확인 단계로 가거나 부팅하고 돌아가요.
 */
import QtQuick
import QtQuick.Layouts
import QtQuick.Dialogs
import Kgc.Ui
import Kgc.FcFlash

Column {
    id: root
    spacing: Theme.panelInset
    readonly property int source: FcFlashController.source
    readonly property string failure: {
        const language = translator.language; const file = FcFlashController.filePath
        const loading = FcFlashController.listLoading; const ready = FcFlashController.canContinue
        return FcFlashController.failureText()
    }
    Text {
        width: parent.width
        wrapMode: Text.Wrap
        text: qsTr("%1 %2 · 보드 ID %3 · 플래시 %4 KB · 부트로더 %5")
            .arg(FcFlashController.boardManufacturer || "—").arg(FcFlashController.boardProduct || "—")
            .arg(FcFlashController.boardId).arg(FcFlashController.flashKb).arg(FcFlashController.bootloaderRevision)
        color: Theme.inkMuted
        font.family: Theme.fontFamily
        font.pixelSize: Theme.fontSize
    }
    PillGroup {
        id: sources
        objectName: "fcFlashSources"
        model: [qsTr("PX4"), qsTr("ArduPilot"), qsTr("파일 선택")]
        currentIndex: root.source
        onActivated: index => FcFlashController.chooseSource(index)
        Connections {
            target: FcFlashController
            function onChanged() { sources.currentIndex = FcFlashController.source }
        }
    }
    ComboField {
        visible: root.source === FlashStates.ArduPilotOfficial
        label: qsTr("기체")
        model: ["Copter", "Heli", "Plane", "Rover", "Sub"]
        currentIndex: model.indexOf(FcFlashController.vehicle)
        enabled: !FcFlashController.listLoading
        onActivated: FcFlashController.chooseVehicle(currentText)
    }
    ComboField {
        objectName: "fcFlashVersions"
        visible: root.source !== FlashStates.LocalFile
        label: qsTr("버전")
        model: { const language = translator.language; return FcFlashController.versions }
        currentIndex: FcFlashController.versionIndex
        enabled: !FcFlashController.listLoading
        onActivated: index => FcFlashController.chooseVersion(index)
    }
    BusySpinner { visible: FcFlashController.listLoading }
    Text {
        visible: FcFlashController.candidates.length > 1 && root.source !== FlashStates.LocalFile
        text: qsTr("맞는 펌웨어 %1개. 직접 골라요.").arg(FcFlashController.candidates.length)
        color: Theme.inkMuted
        font.pixelSize: Theme.fontSize
    }
    DataTable {
        id: table
        objectName: "fcFlashCandidates"
        width: parent.width
        visible: root.source !== FlashStates.LocalFile && FcFlashController.candidates.length > 0
        columns: [qsTr("펌웨어")]
        rows: FcFlashController.candidates.map(value => [value])
        currentIndex: FcFlashController.candidateIndex
        onCurrentIndexChanged: if (currentIndex >= 0 && currentIndex !== FcFlashController.candidateIndex) FcFlashController.chooseCandidate(currentIndex)
        Connections {
            target: FcFlashController
            function onChanged() { if (root.visible) table.currentIndex = FcFlashController.candidateIndex }
        }
    }
    Banner {
        visible: root.source !== FlashStates.LocalFile && !FcFlashController.listLoading
            && (FcFlashController.listFailed || FcFlashController.candidates.length === 0)
        level: "warn"
        text: FcFlashController.listFailed
            ? qsTr("목록을 받지 못했어요. 인터넷을 확인하거나 파일을 직접 골라요.")
            : qsTr("공식 목록에 이 보드용 펌웨어가 없어요. 파일을 직접 골라요.")
    }
    FileField {
        objectName: "fcFlashFile"
        visible: root.source === FlashStates.LocalFile
        text: FcFlashController.filePath
        placeholderText: qsTr(".px4 또는 .apj 파일")
        onBrowseRequested: fileDialog.open()
        onEditingFinished: FcFlashController.chooseFile(text)
    }
    FileDialog {
        id: fileDialog
        title: qsTr("펌웨어 파일 선택")
        nameFilters: [qsTr("펌웨어 파일 (*.px4 *.apj)")]
        onAccepted: {
            let path = decodeURIComponent(selectedFile.toString().slice(7))
            if (!path.startsWith("/")) path = "//" + path
            if (/^\/[A-Za-z]:\//.test(path)) path = path.slice(1)
            FcFlashController.chooseFile(path)
        }
    }
    Text {
        visible: text !== ""
        width: parent.width
        text: root.failure
        color: Theme.danger
        wrapMode: Text.Wrap
        font.pixelSize: Theme.fontSize
    }
    RowLayout {
        width: parent.width
        spacing: Theme.inset
        ActionButton {
            text: qsTr("취소하고 FC 부팅")
            kind: "secondary"
            onClicked: FcFlashController.bootAndReset()
        }
        Item { Layout.fillWidth: true }
        ActionButton {
            objectName: "fcFlashNext"
            text: qsTr("다음")
            enabled: FcFlashController.canContinue
            onClicked: FcFlashController.toConfirm()
        }
    }
}
