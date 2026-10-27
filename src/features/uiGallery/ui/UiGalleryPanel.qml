// [0.0.4 T30 T31] specs/features/0.0.4-righttabs.md
// [0.0.2 T1 T10 T17] specs/features/0.0.2-theme.md
import QtQuick
import QtQuick.Controls.Basic as Basic
import Kgc.Ui
Basic.ScrollView {
    id: panel
    property Item glassSource: null
    contentWidth: availableWidth
    rightPadding: Theme.scrollGutter
    Basic.ScrollBar.vertical.implicitWidth: Theme.scrollBarWidth
    clip: true
    /** 패널을 열면 첫 조작 위치를 테마 선택에 맞춰요.
     * 입력: 사용하지 않는 탐색 인자.
     * 출력: 없음.
     * 실패: 없음.
     * 흐름: 1. galleryThemeSwitch.forceActiveFocus — 테마 선택 줄로 초점을 옮겨요.
     */
    function onArg(arg) { Qt.callLater(() => themeSwitch.forceActiveFocus()) }
    Column {
        width: panel.availableWidth
        spacing: Theme.panelInset
        PillGroup {
            id: themeSwitch
            objectName: "galleryThemeSwitch"
            model: {
                const language = translator.language
                return themeSwitcher.themes.map(theme => qsTranslate("Theme", theme.titleKey))
            }
            currentIndex: themeSwitcher.currentIndex
            onActivated: index => themeSwitcher.choose(themeSwitcher.themes[index].id)
        }
        GalleryButtons { width: parent.width; source: panel.glassSource }
        GalleryChoices { width: parent.width; source: panel.glassSource }
        GalleryInputs { width: parent.width; source: panel.glassSource }
        GalleryIndicators { width: parent.width; source: panel.glassSource }
        GalleryContainers { width: parent.width; source: panel.glassSource }
        GalleryNotices { width: parent.width; source: panel.glassSource }
        GalleryCharts { width: parent.width; source: panel.glassSource }
    }
}
