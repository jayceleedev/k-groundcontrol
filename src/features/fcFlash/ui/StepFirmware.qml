// [0.4.1 6.1–6.4] specs/features/0.4.1-board-match.md
// [0.2.0 T19 T23 T24 T27] specs/features/0.2.0-fcflash.md
/** 확인한 보드에 맞는 공식 펌웨어나 파일을 골라요.
 * 입력: 보드 정보, USB 보드 이름 맞추기 결과, 목록과 파일 해석 결과예요.
 * 출력: 출처·버전·후보 선택과 다음 단계 요청이에요. 인식 결과 줄과, 검색으로 거르는 후보 목록(다른 보드 ID는 회색, 고를 수 없음)을 보여요.
 * 실패: 목록 오류와 파일의 호환 오류를 보여요.
 * 흐름:
 * 1. FcFlashController.chooseSource·chooseVehicle·chooseVersion·chooseCandidate·chooseFile — 선택을 서비스에 넘겨요.
 * 2. FcFlashController.showOtherBoards — "다른 보드 ID도 보기"를 컨트롤러에 남겨요.
 * 3. FcFlashController.toConfirm·bootAndReset — 확인 단계로 가거나 부팅하고 돌아가요.
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
    // 같은 보드 ID 후보 표가 보이는 때예요(0.2.0의 후보 표와 같은 조건).
    readonly property bool listShown: root.source !== FlashStates.LocalFile && !FcFlashController.listLoading
        && !FcFlashController.listFailed && FcFlashController.candidates.length > 0
    readonly property int visibleRows: 8
    readonly property int step: FcFlashController.step
    property var rows: []
    property string rowsKey: ""
    onStepChanged: if (step === FlashStates.Firmware) search.clear()
    /** 맞은 후보 → 나머지 후보 → 다른 보드 순서로 행을 만들고 검색어로 걸러요.
     * 입력: 컨트롤러의 후보·맞은 위치·다른 보드, showOtherBoards, 검색어예요.
     * 출력: rows이며, 값이 같으면 다시 만들지 않아 목록의 스크롤 위치를 지켜요.
     * 실패: 없어요.
     */
    function refreshRows() {
        const candidates = Array.from(FcFlashController.candidates)
        const matched = Array.from(FcFlashController.matchedCandidates)
        const others = FcFlashController.showOtherBoards ? FcFlashController.otherBoards : []
        const query = search.text.trim().toLowerCase()
        const key = JSON.stringify([candidates, matched, others, query, FcFlashController.boardId])
        if (key === rowsKey) return
        rowsKey = key
        const keep = name => query === "" || name.toLowerCase().indexOf(query) >= 0
        const list = []
        const add = index => {
            if (keep(candidates[index])) list.push({name: candidates[index], index: index, boardId: FcFlashController.boardId,
                matched: matched.indexOf(index) >= 0, other: false})
        }
        matched.forEach(add)
        for (let i = 0; i < candidates.length; ++i) if (matched.indexOf(i) < 0) add(i)
        for (const item of others) if (keep(item.name)) list.push({name: item.name, index: -1, boardId: item.boardId, matched: false, other: true})
        rows = list
    }
    Component.onCompleted: refreshRows()
    Connections {
        target: FcFlashController
        function onChanged() { root.refreshRows() }
        function onShowOtherBoardsChanged() { root.refreshRows() }
    }
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
    Column {
        id: matchInfo
        objectName: "fcFlashMatch"
        width: parent.width
        spacing: Theme.inset
        readonly property var matched: FcFlashController.matchedCandidates
        visible: root.listShown && (FcFlashController.matchKind !== FlashStates.NoName || FcFlashController.candidates.length > 1)
        RowLayout {
            width: parent.width
            spacing: Theme.inset
            visible: FcFlashController.matchKind === FlashStates.Matched
            Chip { text: qsTr("인식됨"); level: "ok" }
            Text {
                Layout.fillWidth: true
                wrapMode: Text.Wrap
                text: matchInfo.matched.length === 1
                    ? qsTr("보드 이름 %1에 맞는 빌드: %2").arg(FcFlashController.usbName)
                        .arg(FcFlashController.candidates[matchInfo.matched[0]] ?? "")
                    : qsTr("보드 이름 %1에 맞는 빌드가 %2개예요. 하나를 골라요.").arg(FcFlashController.usbName).arg(matchInfo.matched.length)
                color: Theme.ink
                font.family: Theme.fontFamily
                font.pixelSize: Theme.fontSize
            }
        }
        Banner {
            width: parent.width
            visible: FcFlashController.matchKind === FlashStates.NoMatch
            closable: false; level: "warn"
            text: qsTr("공식 목록에 %1 빌드가 없어요. 아래에서 직접 고르거나 파일을 골라요.").arg(FcFlashController.usbName)
        }
        Text {
            visible: FcFlashController.matchKind === FlashStates.NoName && FcFlashController.candidates.length > 1
            text: qsTr("맞는 펌웨어 %1개. 직접 골라요.").arg(FcFlashController.candidates.length)
            color: Theme.inkMuted
            font.pixelSize: Theme.fontSize
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
    RowLayout {
        width: parent.width
        visible: root.listShown
        spacing: Theme.inset
        SearchBox {
            id: search
            objectName: "fcFlashBoardSearch"
            Layout.fillWidth: true
            placeholderText: qsTr("보드 이름 검색")
            onTextChanged: root.refreshRows()
        }
        CheckRow {
            objectName: "fcFlashOtherBoards"
            tristate: false
            text: qsTr("다른 보드 ID도 보기")
            checked: FcFlashController.showOtherBoards
            onClicked: FcFlashController.showOtherBoards = checked
        }
    }
    BoardList {
        width: parent.width
        visible: root.listShown && root.rows.length > 0
        rows: root.rows
        visibleRows: root.visibleRows
    }
    EmptyState {
        width: parent.width
        visible: root.listShown && root.rows.length === 0
        title: qsTr("검색어와 맞는 보드가 없어요")
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
