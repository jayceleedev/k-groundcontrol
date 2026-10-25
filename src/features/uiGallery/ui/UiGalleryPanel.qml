// [0.0.2 T1 T10 T17] specs/features/0.0.2-theme.md
import QtQuick
import QtQuick.Controls.Basic as Basic
import Kgc.Ui
Basic.ScrollView {
    id: panel
    property Item source: null
    contentWidth: availableWidth
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
            currentIndex: themeSwitcher.themes.findIndex(theme => theme.id === themeSwitcher.themeId)
            onActivated: index => themeSwitcher.choose(themeSwitcher.themes[index].id)
        }
        GalleryButtons { width: parent.width; source: panel.source }
        GalleryChoices { width: parent.width; source: panel.source }
        GalleryInputs { width: parent.width; source: panel.source }
        GalleryIndicators { width: parent.width; source: panel.source }
        GalleryContainers { width: parent.width; source: panel.source }
        GalleryNotices { width: parent.width; source: panel.source }
        GalleryCharts { width: parent.width; source: panel.source }
    }
}
