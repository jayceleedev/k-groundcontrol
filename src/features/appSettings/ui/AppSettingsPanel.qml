// [0.6.0 5.12 T28] specs/features/0.6.0-fly.md
// [0.5.0 5.9 T27] specs/features/0.5.0-map.md
// [0.0.4 T30 T31] specs/features/0.0.4-righttabs.md
// [0.0.2 T4 T14 T15 T20 T24 T25] specs/features/0.0.2-theme.md
import QtQuick
import QtQuick.Dialogs
import QtQuick.Controls
import Kgc.Ui
import Kgc.Map
import Kgc.Fly
SettingsPage {
    id: panel
    categories: [
        {id: "general", title: qsTr("일반")},
        {id: "display", title: qsTr("화면")},
        {id: "input", title: qsTr("조작")},
        {id: "map", title: qsTr("지도")},
        {id: "about", title: qsTr("정보")}
    ]
    /** 탐색 인자에 맞는 설정 카테고리를 열어요.
     * 입력: 카테고리 식별자 또는 빈 인자.
     * 출력: 없음.
     * 실패: 알 수 없는 인자에는 일반 카테고리를 열어요.
     * 흐름:
     * 1. SettingsPage.currentIndex — 탐색한 카테고리를 선택해요.
     */
    function onArg(arg) {
        currentIndex = Math.max(0, categories.findIndex(category => category.id === arg))
    }
    Section {
        width: parent.width
        category: "general"
        ComboField {
            id: languageCombo
            objectName: "languageCombo"
            label: qsTr("언어")
            model: appSettingsController.languages.map(code => code === "system" ? qsTr("시스템 따라감") : appSettingsController.languageName(code))
            currentIndex: appSettingsController.languages.indexOf(appSettingsController.language)
            onActivated: appSettingsController.setLanguage(appSettingsController.languages[currentIndex])
        }
    }
    Section {
        width: parent.width
        category: "display"
        ComboField {
            id: themeCombo
            objectName: "themeCombo"
            label: qsTr("테마")
            model: {
                const language = translator.language
                return themeSwitcher.themes.map(theme => qsTranslate("Theme", theme.titleKey))
            }
            currentIndex: themeSwitcher.currentIndex
            onActivated: themeSwitcher.choose(themeSwitcher.themes[currentIndex].id)
        }
        Label { text: qsTr("배경"); color: Theme.inkMuted }
        PillGroup {
            objectName: "backgroundKindPicker"
            model: [qsTr("민무늬"), qsTr("체크무늬"), qsTr("사진")]
            // 단추의 내부 선택 쓰기 뒤에도 파일 선택으로 바뀐 종류를 다시 반영해요.
            Binding on currentIndex {
                value: uiSettingValues.backgroundKinds.indexOf(uiSettingValues.backgroundKind)
            }
            onActivated: index => { uiSettingValues.backgroundKind = uiSettingValues.backgroundKinds[index] }
        }
        Label { text: qsTr("배경 사진"); color: Theme.inkMuted }
        FileField {
            objectName: "backgroundImageField"
            text: uiSettingValues.backgroundImage
            onEditingFinished: uiSettingValues.chooseBackgroundImage(text)
            onBrowseRequested: backgroundDialog.open()
        }
    }
    Section {
        width: parent.width
        category: "input"
        Label { text: qsTr("길게 눌러 확인 시간"); color: Theme.inkMuted }
        NumberBox {
            objectName: "holdTimeField"
            value: uiSettingValues.holdToConfirmMs
            from: uiSettingValues.holdToConfirmMsMin
            to: uiSettingValues.holdToConfirmMsMax
            unit: qsTr("ms")
            onValueModified: value => { uiSettingValues.holdToConfirmMs = value }
        }
        // 유도 한계는 vehicle/safety의 설정이라 FlyController를 거쳐 읽고 써요(범위 밖 값은 저장되지 않아요).
        Label { text: qsTr("유도 비행 최대 거리"); color: Theme.inkMuted }
        NumberBox {
            objectName: "guidedMaxDistanceField"
            value: FlyController.guidedMaxDistanceM
            from: FlyController.guidedMaxDistanceMMin
            to: FlyController.guidedMaxDistanceMMax
            decimals: 0
            unit: qsTr("m")
            onValueModified: value => { FlyController.guidedMaxDistanceM = value }
        }
        Label { text: qsTr("유도 비행 최저 고도"); color: Theme.inkMuted }
        NumberBox {
            objectName: "guidedMinAltitudeField"
            value: FlyController.guidedMinAltitudeM
            from: FlyController.guidedMinAltitudeMMin
            to: FlyController.guidedMinAltitudeMMax
            decimals: 1
            unit: qsTr("m")
            onValueModified: value => { FlyController.guidedMinAltitudeM = value }
        }
        Label { text: qsTr("유도 비행 최고 고도"); color: Theme.inkMuted }
        NumberBox {
            objectName: "guidedMaxAltitudeField"
            value: FlyController.guidedMaxAltitudeM
            from: FlyController.guidedMaxAltitudeMMin
            to: FlyController.guidedMaxAltitudeMMax
            decimals: 2
            unit: qsTr("m")
            onValueModified: value => { FlyController.guidedMaxAltitudeM = value }
        }
    }
    // 지도 설정은 지도 모듈의 설정이라 MapController를 거쳐 읽고 써요. 저장 크기 보기·비우기는 지도 저장 카드에 있어요.
    Section {
        width: parent.width
        category: "map"
        Label { text: qsTr("본 타일 저장 한도"); color: Theme.inkMuted }
        NumberBox {
            objectName: "mapStoreLimitField"
            value: MapController.tileStoreLimitMb
            from: MapController.tileStoreLimitMbMin
            to: MapController.tileStoreLimitMbMax
            unit: qsTr("MB")
            onValueModified: value => { MapController.tileStoreLimitMb = value }
        }
        Label { text: qsTr("궤적 점 간격"); color: Theme.inkMuted }
        NumberBox {
            objectName: "mapTrailSpacingField"
            value: MapController.trailMinMeters
            from: MapController.trailMinMetersMin
            to: MapController.trailMinMetersMax
            decimals: 1
            stepSize: 0.1
            unit: qsTr("m")
            onValueModified: value => { MapController.trailMinMeters = value }
        }
        Label { text: qsTr("궤적 최대 점 수"); color: Theme.inkMuted }
        NumberBox {
            objectName: "mapTrailLengthField"
            value: MapController.trailMaxPoints
            from: MapController.trailMaxPointsMin
            to: MapController.trailMaxPointsMax
            unit: qsTr("점")
            onValueModified: value => { MapController.trailMaxPoints = value }
        }
        Toggle {
            objectName: "mapAutoFollowToggle"
            text: qsTr("기체 자동 따라가기")
            checked: MapController.autoFollow
            onToggled: MapController.autoFollow = checked
        }
    }
    Section {
        width: parent.width
        category: "about"
        Row {
            spacing: Theme.inset
            Label { text: qsTr("버전"); color: Theme.inkMuted }
            Label { objectName: "aboutVersion"; text: appSettingsController.version; color: Theme.ink }
        }
        ActionButton { objectName: "aboutLicenseButton"; text: qsTr("라이선스 보기"); onClicked: licenseDialog.open() }
        ActionButton { objectName: "aboutNoticesButton"; text: qsTr("고지 보기"); onClicked: noticesDialog.open() }
        Label { text: qsTr("소스"); color: Theme.inkMuted }
        TextBox {
            objectName: "aboutSourceUrl"
            width: parent.width
            text: appSettingsController.sourceUrl
            readOnly: true
            color: Theme.ink
            selectByMouse: true
        }
    }
    FileDialog {
        id: backgroundDialog
        title: qsTr("배경 사진")
        nameFilters: [qsTr("사진 (*.png *.jpg *.jpeg)")]
        onAccepted: uiSettingValues.chooseBackgroundImage(selectedFile.toString())
    }
    Dialog {
        id: licenseDialog
        objectName: "licenseDialog"
        parent: Overlay.overlay
        anchors.centerIn: parent
        width: Math.min(Theme.dialogWidth, parent.width - Theme.panelInset)
        height: Math.min(Theme.dialogHeight, parent.height - Theme.panelInset)
        modal: true
        background: Rectangle { color: Theme.surface; border.color: Theme.line; radius: Theme.radiusMedium }
        title: qsTr("라이선스 보기")
        footer: DialogButtonBox { ActionButton { text: qsTr("닫기"); onClicked: licenseDialog.close() } }
        contentItem: ScrollView {
            clip: true
            rightPadding: Theme.scrollGutter
            contentWidth: availableWidth
            ScrollBar.vertical.implicitWidth: Theme.scrollBarWidth
            TextArea {
                objectName: "licenseText"
                text: appSettingsController.licenseText
                readOnly: true
                selectByMouse: true
                color: Theme.ink
                background: Rectangle { color: Theme.surface }
                wrapMode: TextEdit.Wrap
            }
        }
    }
    Dialog {
        id: noticesDialog
        objectName: "noticesDialog"
        parent: Overlay.overlay
        anchors.centerIn: parent
        width: Math.min(Theme.dialogWidth, parent.width - Theme.panelInset)
        height: Math.min(Theme.dialogHeight, parent.height - Theme.panelInset)
        modal: true
        background: Rectangle { color: Theme.surface; border.color: Theme.line; radius: Theme.radiusMedium }
        title: qsTr("고지 보기")
        footer: DialogButtonBox { ActionButton { text: qsTr("닫기"); onClicked: noticesDialog.close() } }
        contentItem: ScrollView {
            clip: true
            rightPadding: Theme.scrollGutter
            contentWidth: availableWidth
            ScrollBar.vertical.implicitWidth: Theme.scrollBarWidth
            TextArea {
                objectName: "noticesText"
                text: appSettingsController.noticesText
                readOnly: true
                selectByMouse: true
                color: Theme.ink
                background: Rectangle { color: Theme.surface }
                wrapMode: TextEdit.Wrap
            }
        }
    }
}
