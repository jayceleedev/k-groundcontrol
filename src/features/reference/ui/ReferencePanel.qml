// [0.0.4 T41 T42 T43] specs/features/0.0.4-righttabs.md
/** 설정에 필요한 공식 문서의 소개와 바로가기를 모아 보여요.
 * 입력: 유리 바탕으로 쓸 glassSource와 사용하지 않는 탐색 인자.
 * 출력: 세로로 스크롤하는 사이트 카드 여섯 개.
 * 실패: 브라우저를 열지 못했을 때 별도 안내는 하지 않아요.
 * 흐름:
 * 1. ReferenceCard의 ActionButton.onClicked → Qt.openUrlExternally — 고른 사이트의 주소를 기본 브라우저에 넘겨요.
 */
import QtQuick
import QtQuick.Controls.Basic as Basic
import Kgc.Ui

Basic.ScrollView {
    id: panel
    objectName: "referencePanel"
    property Item glassSource: null
    contentWidth: availableWidth
    rightPadding: Theme.scrollGutter
    Basic.ScrollBar.vertical.implicitWidth: Theme.scrollBarWidth
    clip: true
    readonly property var sites: [
        {
            siteId: "ardupilot", name: "ArduPilot",
            description: qsTr("멀티콥터, 비행기, 로버 등을 위한 오픈소스 자동조종 펌웨어 공식 문서. 기체 설정과 파라미터 안내"),
            address: "https://ardupilot.org/ardupilot/"
        },
        {
            siteId: "px4", name: "PX4",
            description: qsTr("PX4 자동조종 펌웨어 공식 사용자 안내서. 기체 조립, 설정, 비행 방법 안내"),
            address: "https://docs.px4.io/main/en/"
        },
        {
            siteId: "am32", name: "AM32",
            description: qsTr("브러시리스 모터 ESC용 오픈소스 펌웨어. 펌웨어 받기와 웹 설정 도구 제공"),
            address: "https://am32.ca/"
        },
        {
            siteId: "bluejay", name: "Bluejay",
            description: qsTr("BLHeli_S ESC용 오픈소스 디지털 펌웨어. GitHub에서 소스, 릴리스, 설정 정보 제공"),
            address: "https://github.com/bird-sanctuary/bluejay"
        },
        {
            siteId: "expresslrs", name: "ExpressLRS",
            description: qsTr("오픈소스 무선 조종 링크. 송수신기 설정과 지원 장비 안내"),
            address: "https://www.expresslrs.org/"
        },
        {
            siteId: "mavlink", name: "MAVLink",
            description: qsTr("지상국과 기체 간 메시지 규격의 개발자 안내서. 메시지와 명령 안내"),
            address: "https://mavlink.io/en/"
        }
    ]
    /** 탐색 인자가 없는 참고 패널에 키보드 초점을 맞춰요.
     * 입력: 사용하지 않는 탐색 인자.
     * 출력: 없음.
     * 실패: 없음.
     */
    function onArg(arg) { forceActiveFocus() }
    Column {
        width: panel.availableWidth
        spacing: Theme.panelInset
        Text {
            width: parent.width
            text: qsTr("설정 참고용 공식 문서. 바로가기 선택 시 브라우저에서 열기")
            wrapMode: Text.Wrap
            color: Theme.inkMuted
            font.family: Theme.fontFamily
            font.pixelSize: Theme.fontSize
        }
        Repeater {
            model: panel.sites
            delegate: ReferenceCard {
                required property var modelData
                width: parent.width
                siteId: modelData.siteId
                title: modelData.name
                description: modelData.description
                address: modelData.address
                source: panel.glassSource
            }
        }
    }
}
