// [0.2.0 T19 T23 T24 T27] specs/features/0.2.0-fcflash.md
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
    BoardCard { width: parent.width }
    Row {
        id: sources
        objectName: "fcFlashSources"
        width: parent.width
        spacing: Theme.inset
        Repeater {
            id: sourceCards
            // 순서는 FirmwareSource 값과 같아서 index를 그대로 서비스에 넘겨요.
            model: [
                {icon: "rocket", title: qsTr("PX4"), subtitle: qsTr("공식 릴리스")},
                {icon: "drone", title: qsTr("ArduPilot"), subtitle: qsTr("공식 릴리스")},
                {icon: "file-upload", title: qsTr("파일 선택"), subtitle: qsTr(".px4 · .apj")}
            ]
            ChoiceCard {
                required property var modelData
                required property int index
                objectName: "fcFlashSource_" + index
                width: (sources.width - sources.spacing * (sourceCards.count - 1)) / sourceCards.count
                icon.name: modelData.icon
                title: modelData.title
                subtitle: modelData.subtitle
                selected: root.source === index
                onClicked: FcFlashController.chooseSource(index)
            }
        }
    }
    Row {
        id: vehicles
        width: parent.width
        visible: root.source === FlashStates.ArduPilotOfficial
        spacing: Theme.inset
        Repeater {
            id: vehicleCards
            model: [
                {name: "Copter", icon: "drone"}, {name: "Heli", icon: "helicopter"}, {name: "Plane", icon: "plane"},
                {name: "Rover", icon: "car"}, {name: "Sub", icon: "submarine"}
            ]
            ChoiceCard {
                required property var modelData
                objectName: "fcFlashVehicle_" + modelData.name
                width: (vehicles.width - vehicles.spacing * (vehicleCards.count - 1)) / vehicleCards.count
                compact: true
                icon.name: modelData.icon
                subtitle: modelData.name
                selected: FcFlashController.vehicle === modelData.name
                enabled: !FcFlashController.listLoading
                onClicked: FcFlashController.chooseVehicle(modelData.name)
            }
        }
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
