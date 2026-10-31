// [0.2.0 T9] specs/features/0.2.0-fcflash.md
#pragma once
#include <QByteArray>
#include <QList>
#include <QString>
namespace kgc::fcFlash {
/** ArduPilot 기체 콤보의 값이에요.
 * 입력: 없어요.
 * 출력: Copter·Heli·Plane·Rover·Sub 중 하나예요.
 * 실패: 없어요.
 */
enum class ArduPilotVehicle { Copter, Heli, Plane, Rover, Sub };
/** 목록에서 고른 펌웨어 하나예요.
 * 입력: scanManifest가 채워요.
 * 출력: 기체, buildType(OFFICIAL 또는 STABLE-x.y.z), major.minor.patch 버전, 후보 표시 platform, 내려받을 url이에요.
 * 실패: 없어요.
 */
struct ManifestEntry {
    ArduPilotVehicle vehicle = ArduPilotVehicle::Copter;
    QString buildType;
    QString version;
    QString platform;
    QString url;
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
/** manifest.json에서 이 보드에 맞는 항목만 골라요.
 * 입력: 풀린 목록 bytes와 보드 ID boardId예요. 보드 255는 9로 봐요.
 * 출력: format이 apj이고, board_id가 맞고, 다섯 기체 중 하나이고, 빌드 종류가 OFFICIAL 또는 STABLE-x.y.z인 항목이에요.
 *       Heli는 mav-type이 HELICOPTER인 항목, Copter는 vehicletype이 Copter이고 HELICOPTER가 아닌 항목이에요.
 *       86MB를 한 번에 QJsonDocument로 만들지 않고 항목 단위로 잘라 맞는 것만 해석해요.
 * 실패: 읽을 수 없는 항목은 건너뛰어요.
 */
QList<ManifestEntry> scanManifest(const QByteArray& bytes, int boardId);
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
}
