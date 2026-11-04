// [0.4.1 5.3 5.4 T3] specs/features/0.4.1-board-match.md
// [0.2.0 T9] specs/features/0.2.0-fcflash.md
#pragma once
#include "BoardMatch.h"
#include <QByteArray>
#include <QList>
#include <QString>
#include <QStringList>
namespace kgc::fcFlash {
/** ArduPilot 기체 콤보의 값이에요.
 * 입력: 없어요.
 * 출력: Copter·Heli·Plane·Rover·Sub 중 하나예요.
 * 실패: 없어요.
 */
enum class ArduPilotVehicle { Copter, Heli, Plane, Rover, Sub };
/** 목록에서 고른 펌웨어 하나예요.
 * 입력: scanManifest가 채워요.
 * 출력: 기체, buildType(OFFICIAL 또는 STABLE-x.y.z), major.minor.patch 버전, 후보 표시 platform, 내려받을 url,
 *       보드 이름 맞추기에 쓰는 bootloader_str 값 bootloaderNames예요.
 * 실패: 없어요.
 */
struct ManifestEntry {
    ArduPilotVehicle vehicle = ArduPilotVehicle::Copter;
    QString buildType;
    QString version;
    QString platform;
    QString url;
    QStringList bootloaderNames;
};
/** 보드 ID가 다른 목록 항목 하나예요.
 * 입력: scanManifest가 채워요.
 * 출력: platform, 그 항목의 board_id인 boardId, 기체, 빌드 종류예요.
 * 실패: 없어요.
 */
struct ManifestBoard {
    QString platform;
    int boardId = 0;
    ArduPilotVehicle vehicle = ArduPilotVehicle::Copter;
    QString buildType;
};
/** 목록 한 번 해석의 결과예요.
 * 입력: scanManifest가 채워요.
 * 출력: 이 보드에 맞는 항목 entries와 보드 ID가 다른 항목 boards예요.
 * 실패: 없어요.
 */
struct ManifestScan {
    QList<ManifestEntry> entries;
    QList<ManifestBoard> boards;
};
/** 버전 콤보의 항목 하나예요.
 * 입력: manifestVersions가 채워요.
 * 출력: buildType과 표시할 version이에요.
 * 실패: 없어요.
 */
struct ManifestVersion {
    QString buildType;
    QString version;
};
/** manifest.json을 한 번 훑어 이 보드의 항목과 다른 보드의 항목을 나눠요.
 * 입력: 풀린 목록 bytes와 보드 ID boardId예요. 보드 255는 9로 봐요.
 * 출력: entries는 format이 apj이고, board_id가 맞고, 다섯 기체 중 하나이고, 빌드 종류가 OFFICIAL 또는 STABLE-x.y.z인 항목이에요.
 *       Heli는 mav-type이 HELICOPTER인 항목, Copter는 vehicletype이 Copter이고 HELICOPTER가 아닌 항목이에요.
 *       boards는 board_id가 compatible하지 않은 apj 항목 중 같은 기체·빌드 종류 조건을 통과한 것이고,
 *       (platform, boardId, 기체, 빌드 종류)가 같으면 한 번만 넣어요.
 *       86MB를 한 번에 QJsonDocument로 만들지 않고 항목 단위로 잘라 맨 위 필드만 바로 읽어요. entries 항목만 객체로 해석해요.
 * 실패: 읽을 수 없는 항목은 건너뛰어요.
 */
ManifestScan scanManifest(const QByteArray& bytes, int boardId);
/** 기체의 버전 콤보 순서를 만들어요.
 * 입력: scanManifest의 entries와 기체 vehicle이에요.
 * 출력: OFFICIAL이 먼저, 그다음 STABLE-x.y.z가 수 비교로 최신 순이에요. 빌드 종류마다 하나씩이에요.
 * 실패: 없어요.
 */
QList<ManifestVersion> manifestVersions(const QList<ManifestEntry>& entries, ArduPilotVehicle vehicle);
/** 기체와 빌드 종류에 맞는 후보를 골라요.
 * 입력: entries, vehicle, buildType이에요.
 * 출력: 조건에 맞는 항목 전부예요.
 * 실패: 없어요.
 */
QList<ManifestEntry> manifestCandidates(const QList<ManifestEntry>& entries, ArduPilotVehicle vehicle, const QString& buildType);
/** 고른 기체·빌드 종류에서 보드 ID가 다른 보드의 행을 만들어요.
 * 입력: scanManifest의 결과 scan, vehicle, buildType이에요.
 * 출력: 그 기체·빌드 종류의 boards를 {platform, boardId}로 바꾼 행이에요. 같은 보드 ID 후보와 platform이 같은 행은 빼고,
 *       한 platform이 여러 보드 ID에 있으면 보드 ID마다 한 행이에요. 이름을 대소문자 무시로, 같으면 보드 ID 작은 순으로 정렬해요.
 * 실패: 없어요.
 */
QList<OtherBoard> manifestOtherBoards(const ManifestScan& scan, ArduPilotVehicle vehicle, const QString& buildType);
}
