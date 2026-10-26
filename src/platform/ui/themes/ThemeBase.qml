// [0.0.2 T2–T9 T16 T20] specs/features/0.0.2-theme.md
// 테마는 이 파일의 required 토큰을 모두 채운 파일 하나로 더해요.
// 식별자는 파일 이름의 첫 글자를 소문자로 바꾸고, order가 작을수록 먼저 나와요.
// [0.0.4 T9 T9a] specs/features/0.0.4-righttabs.md
import QtQuick
QtObject {
    required property color series1
    required property color series2
    required property color series3
    required property color series4
    required property color series5
    required property color series6
    required property color chartGrid
    required property color chartAxis
    required property color dangerInk
    required property color window
    required property color region
    required property color surface
    required property color surfaceAlt
    required property color overlay
    required property color ink
    required property color inkMuted
    required property color inkInverse
    required property color inkDisabled
    required property color line
    required property color lineStrong
    required property color focus
    required property color accent
    required property color accentHover
    required property color accentPressed
    required property color accentInk
    required property color ok
    required property color warn
    required property color danger
    required property color info
    required property color idle
    required property color okSoft
    required property color warnSoft
    required property color dangerSoft
    required property color infoSoft
    required property color glassTint
    required property real glassBlur
    required property int glassBlurMax
    required property color glassEdge
    required property real glassShine
    required property int radiusSmall
    required property int radiusMedium
    required property int radiusLarge
    required property color shadowColor
    required property color mapPatternA
    required property color mapPatternB
    // 계기(AttitudeIndicator)의 하늘·땅 색이에요.
    required property color attitudeSky
    required property color attitudeGround
    required property int order
    required property bool dark
    required property bool glass
    required property bool shadow
    required property string titleKey
    required property string fontFamily
    required property string monoFamily
}
