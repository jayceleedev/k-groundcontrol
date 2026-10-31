// [0.1.0 T9 T14 T15 T16 T17 T20] specs/features/0.1.0-link.md
import QtQuick
import Kgc.Ui
import Kgc.Link
Chip {
    objectName: "statusChip_link.connection"
    text: {
        const language = translator.language
        switch (LinkController.connectionState) {
        case LinkService.Idle: return qsTr("기체 연결")
        case LinkService.Failed: return qsTr("연결 실패: %1").arg(LinkController.failureText())
        case LinkService.Opening: return qsTr("기체 찾는 중")
        default:
            if (LinkController.sysid < 0) return qsTr("기체 찾는 중")
            if (LinkController.lost) return qsTr("기체 응답 없음")
            return qsTr("%1 · 시스템 %2").arg(LinkController.firmwareName).arg(LinkController.sysid)
        }
    }
    level: {
        if (LinkController.connectionState === LinkService.Failed) return "danger"
        if (LinkController.connectionState === LinkService.Idle) return "idle"
        if (LinkController.connectionState === LinkService.Opening || LinkController.sysid < 0) return "warn"
        return LinkController.lost ? "danger" : "ok"
    }
}
