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
            description: qsTr("멀티콥터, 비행기, 로버 등을 위한 오픈소스 자동조종 펌웨어의 공식 문서예요. 기체 설정과 파라미터를 찾아볼 수 있어요."),
            address: "https://ardupilot.org/ardupilot/"
        },
        {
            siteId: "px4", name: "PX4",
            description: qsTr("PX4 자동조종 펌웨어의 공식 사용자 안내서예요. 기체 조립, 설정, 비행 방법을 다뤄요."),
            address: "https://docs.px4.io/main/en/"
        },
        {
            siteId: "am32", name: "AM32",
            description: qsTr("브러시리스 모터 ESC용 오픈소스 펌웨어예요. 펌웨어 내려받기와 웹 설정 도구가 있어요."),
            address: "https://am32.ca/"
        },
        {
            siteId: "bluejay", name: "Bluejay",
            description: qsTr("BLHeli_S 계열 ESC용 오픈소스 디지털 펌웨어예요. 소스, 릴리스, 설정 정보를 GitHub에서 볼 수 있어요."),
            address: "https://github.com/bird-sanctuary/bluejay"
        },
        {
            siteId: "expresslrs", name: "ExpressLRS",
            description: qsTr("오픈소스 무선 조종 링크예요. 송수신기 설정 안내와 지원 장비를 찾아볼 수 있어요."),
            address: "https://www.expresslrs.org/"
        },
        {
            siteId: "mavlink", name: "MAVLink",
            description: qsTr("지상국과 기체가 주고받는 메시지 규격의 개발자 안내서예요. 메시지와 명령의 뜻을 찾아볼 수 있어요."),
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
            text: qsTr("설정할 때 참고할 공식 문서예요. 바로가기를 누르면 브라우저에서 열려요.")
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
