// [0.0.2 T8 T10–T14 T17 T22 T23] specs/features/0.0.2-theme.md
/** 아이콘의 모양을 테마 색으로 칠해요.
 * 입력: name은 icons.txt의 경로이며 color와 source를 바꿀 수 있어요.
 * 출력: 화면과 해당 부품의 변경 시그널이에요.
 * 실패: 잘못된 입력은 표시하지 않거나 범위 안으로 제한해요.
 */
import QtQuick
import QtQuick.Controls.Basic as Basic
import Kgc.Ui
import QtQuick.Effects

Item {
    id: glyph
    property string name: ""
    property color color: Theme.ink
    property url source: name ? "qrc:/kgc/icons/" + name + ".svg" : ""
    implicitWidth: Theme.iconSize
    implicitHeight: Theme.iconSize
    Image { id: image; anchors.fill: parent; source: glyph.source; fillMode: Image.PreserveAspectFit; visible: false }
    MultiEffect { anchors.fill: image; source: image; brightness: 1.0; colorization: 1.0; colorizationColor: glyph.color }
}
