// [0.0.2 T2–T9 T16 T20] specs/features/0.0.2-theme.md
// 테마는 이 파일의 required 토큰을 모두 채운 파일 하나로 더해요.
// 식별자는 파일 이름의 첫 글자를 소문자로 바꾸고, order가 작을수록 먼저 나와요.
// [0.0.4 T9 T9a T16] specs/features/0.0.4-righttabs.md
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
    // 계기 모양 넷(둥근·PFD·HUD·3D)이 함께 쓰는 색이에요. 지금은 네 테마가 같은 값을 줘요.
    // 눈금·피치 사다리·roll 호, 속도·고도·방위 띠 바탕, PFD 글자와 그 그림자, 노란 고정 기체 표시예요.
    required property color attitudeMark
    required property color attitudeTape
    required property color osdInk
    required property color osdShadow
    required property color attitudeAircraft
    // HUD의 어두운 바탕과 초록 선·글자예요.
    required property color hudBackground
    required property color hudInk
    // 기체 3D의 몸체·기수 표시 색, 나침반 고리와 자북 N 색이에요.
    required property color modelBody
    required property color modelAccent
    required property color compassRing
    required property color compassNorthColor
    required property int order
    required property bool dark
    required property bool glass
    required property bool shadow
    required property string titleKey
    required property string fontFamily
    required property string monoFamily
}
