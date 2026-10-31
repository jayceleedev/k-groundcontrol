// [0.1.0 T1–T6] specs/features/0.1.0-link.md
#pragma once
#include <QString>
#include <QList>
#include <cstdint>
namespace kgc::vehicle::firmware {
/** 요청할 스트림 번호와 필요한 수신 빈도를 전달해요.
 * 입력: 스트림 식별자 streamId와 초당 횟수 rateHz예요.
 * 출력: 송신 문이 REQUEST_DATA_STREAM을 만들 때 사용할 두 값이에요.
 * 실패: 없어요.
 */
struct StreamRequest { std::uint8_t streamId; std::uint16_t rateHz; };
/** 펌웨어별 표시와 스트림 요청 데이터를 제공해요.
 * 입력: 모드 조회의 customMode·mavType이에요.
 * 출력: 공식 펌웨어·모드 이름과 초기 스트림 요청 목록이에요.
 * 실패: 모르는 모드 이름은 빈 문자열로 돌려줘요.
 */
class FirmwarePlugin {
public:
/** 플러그인 자원을 정리해요.
 * 입력: 없어요.
 * 출력: 기체가 소유하던 플러그인의 자원을 해제해요.
 * 실패: 없어요.
 */
    virtual ~FirmwarePlugin() = default;
/** 공식 펌웨어 이름을 돌려줘요.
 * 입력: 없어요.
 * 출력: PX4·ArduPilot 또는 범용 플러그인의 MAVLink 이름이에요.
 * 실패: 없어요.
 */
    virtual QString displayName() const = 0;
/** 모드 번호를 공식 이름으로 바꿔요.
 * 입력: 사용자 모드 번호 customMode와 기체 종류 mavType이에요.
 * 출력: 알려진 모드의 공식 이름이며, 모르는 조합은 빈 문자열이에요.
 * 실패: 없어요.
 */
    virtual QString modeName(std::uint32_t customMode, std::uint8_t mavType) const = 0;
/** 연결할 때 요청할 스트림 목록을 돌려줘요.
 * 입력: 없어요.
 * 출력: ArduPilot의 상태·위치·자세 요청 목록이며, PX4와 범용 플러그인은 빈 목록이에요.
 * 실패: 없어요.
 */
    virtual QList<StreamRequest> streamRequests() const = 0;
};
}
