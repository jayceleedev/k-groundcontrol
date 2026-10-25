// [0.0.2 T4 T14 T15 T20] specs/features/0.0.2-theme.md
import QtQuick
// [0.0.2 T24] specs/features/0.0.2-theme.md
import QtQuick.Dialogs
import QtQuick.Controls
import Kgc.Ui
ScrollView {
    id: panel
    clip: true
    contentWidth: availableWidth
    function onArg(arg) {
        if (arg === "about")
            contentItem.contentY = Math.max(0, Math.min(about.y, contentHeight - availableHeight))
    }
    Column {
        width: panel.availableWidth
        spacing: Theme.panelInset
        ComboField {
            id: languageCombo
            objectName: "languageCombo"
            label: qsTr("언어")
            model: appSettingsController.languages.map(code => code === "system" ? qsTr("시스템 따라감") : appSettingsController.languageName(code))
            currentIndex: appSettingsController.languages.indexOf(appSettingsController.language)
            onActivated: appSettingsController.setLanguage(appSettingsController.languages[currentIndex])
        }
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
        Label { text: qsTr("길게 눌러 확인 시간"); color: Theme.inkMuted }
        NumberBox {
            objectName: "holdTimeField"
            value: uiSettingValues.holdToConfirmMs
            from: uiSettingValues.holdToConfirmMsMin
            to: uiSettingValues.holdToConfirmMsMax
            unit: qsTr("ms")
            onValueModified: value => { uiSettingValues.holdToConfirmMs = value }
        }
        Column {
            id: about
            width: parent.width
            spacing: Theme.inset
            Label { text: qsTr("정보"); font.bold: true; color: Theme.ink }
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
