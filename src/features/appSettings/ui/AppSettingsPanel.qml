// [0.0.2 T4 T14 T15 T20 T24 T25] specs/features/0.0.2-theme.md
import QtQuick
import QtQuick.Dialogs
import QtQuick.Controls
import Kgc.Ui
SettingsPage {
    id: panel
    categories: [
        {id: "general", title: qsTr("일반")},
        {id: "display", title: qsTr("화면")},
        {id: "input", title: qsTr("조작")},
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
        visible: panel.currentCategory === "general"
        text: qsTr("일반")
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
        visible: panel.currentCategory === "display"
        text: qsTr("화면")
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
            currentIndex: uiSettingValues.backgroundKinds.indexOf(uiSettingValues.backgroundKind)
            onActivated: index => { uiSettingValues.backgroundKind = uiSettingValues.backgroundKinds[index] }
        }
        Label { text: qsTr("배경 사진"); color: Theme.inkMuted }
        FileField {
            objectName: "backgroundImageField"
            text: uiSettingValues.backgroundImage
            onEditingFinished: uiSettingValues.backgroundImage = text
            onBrowseRequested: backgroundDialog.open()
        }
    }
    Section {
        width: parent.width
        visible: panel.currentCategory === "input"
        text: qsTr("조작")
        Label { text: qsTr("길게 눌러 확인 시간"); color: Theme.inkMuted }
        NumberBox {
            objectName: "holdTimeField"
            value: uiSettingValues.holdToConfirmMs
            from: uiSettingValues.holdToConfirmMsMin
            to: uiSettingValues.holdToConfirmMsMax
            unit: qsTr("ms")
            onValueModified: value => { uiSettingValues.holdToConfirmMs = value }
        }
    }
    Section {
        width: parent.width
        visible: panel.currentCategory === "about"
        text: qsTr("정보")
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
        onAccepted: {
            uiSettingValues.backgroundImage = selectedFile.toString()
            uiSettingValues.backgroundKind = "image"
        }
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
