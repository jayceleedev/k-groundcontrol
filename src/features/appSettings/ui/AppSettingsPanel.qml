// [0.0.1 T4–T9] specs/features/0.0.1-shell.md
import QtQuick
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
        Label { text: qsTr("언어"); font.bold: true; color: Theme.ink }
        ComboBox {
            id: languageCombo
            objectName: "languageCombo"
            width: Theme.languageWidth
            model: appSettingsController.languages
            currentIndex: model.indexOf(appSettingsController.language)
            displayText: currentValue === "system" ? qsTr("시스템 따라감") : appSettingsController.languageName(currentValue || "")
            delegate: ItemDelegate {
                required property string modelData
                required property int index
                width: languageCombo.width
                text: modelData === "system" ? qsTr("시스템 따라감") : appSettingsController.languageName(modelData)
                highlighted: languageCombo.highlightedIndex === index
            }
            onActivated: appSettingsController.setLanguage(currentValue)
        }
        Column {
            id: about
            width: parent.width
            spacing: Theme.inset
            Label { text: qsTr("정보"); font.bold: true; color: Theme.ink }
            Row {
                spacing: Theme.inset
                Label { text: qsTr("버전"); color: Theme.dim }
                Label { objectName: "aboutVersion"; text: appSettingsController.version; color: Theme.ink }
            }
            Button { objectName: "aboutLicenseButton"; text: qsTr("라이선스 보기"); onClicked: licenseDialog.open() }
            Button { objectName: "aboutNoticesButton"; text: qsTr("고지 보기"); onClicked: noticesDialog.open() }
            Label { text: qsTr("소스"); color: Theme.dim }
            TextArea {
                objectName: "aboutSourceUrl"
                width: parent.width
                text: appSettingsController.sourceUrl
                readOnly: true
                wrapMode: TextEdit.WrapAnywhere
                color: Theme.ink
                selectByMouse: true
            }
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
        title: qsTr("라이선스 보기")
        footer: DialogButtonBox { Button { text: qsTr("닫기"); onClicked: licenseDialog.close() } }
        contentItem: ScrollView {
            clip: true
            TextArea {
                objectName: "licenseText"
                text: appSettingsController.licenseText
                readOnly: true
                selectByMouse: true
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
        title: qsTr("고지 보기")
        footer: DialogButtonBox { Button { text: qsTr("닫기"); onClicked: noticesDialog.close() } }
        contentItem: ScrollView {
            clip: true
            TextArea {
                objectName: "noticesText"
                text: appSettingsController.noticesText
                readOnly: true
                selectByMouse: true
                wrapMode: TextEdit.Wrap
            }
        }
    }
}
