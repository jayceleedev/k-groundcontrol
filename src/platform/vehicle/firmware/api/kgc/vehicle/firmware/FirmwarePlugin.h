// [0.3.0 5.2] specs/features/0.3.0-params.md
// [0.1.0 T1–T6] specs/features/0.1.0-link.md
#pragma once
#include <QString>
#include <QStringList>
#include <QList>
#include <QUrl>
#include <cstdint>
namespace kgc::vehicle::firmware {
/** 요청할 스트림 번호와 필요한 수신 빈도를 전달해요.
 * 입력: 스트림 식별자 streamId와 초당 횟수 rateHz예요.
 * 출력: 송신 문이 REQUEST_DATA_STREAM을 만들 때 사용할 두 값이에요.
 * 실패: 없어요.
 */
struct StreamRequest { std::uint8_t streamId; std::uint16_t rateHz; };
/** 파라미터 값을 PARAM_VALUE의 float 칸에 담는 방식을 나타내요.
 * 입력: 없어요.
 * 출력: CCast는 숫자를 float로 옮기고, Bytewise는 정수의 비트를 float 칸에 그대로 담아요.
 * 실패: 없어요.
 */
enum class ParamEncoding { CCast, Bytewise };
/** 파라미터 메타데이터 파일의 모양을 나타내요.
 * 입력: 없어요.
 * 출력: None은 출처 없음, Flat은 PX4 목록형, Grouped는 ArduPilot 묶음형이에요.
 * 실패: 없어요.
 */
enum class ParamMetadataFormat { None, Flat, Grouped };
/** 파라미터 메타데이터를 어디서 받을지 전달해요.
 * 입력: 파일 모양 format, 차례로 시도할 주소 urls, 캐시 파일 이름의 가운데 cacheName이에요.
 * 출력: 기능 모듈이 내려받을 때 쓰는 값이에요.
 * 실패: 없어요.
 */
struct ParamMetadataSource { ParamMetadataFormat format; QList<QUrl> urls; QString cacheName; };
/** 펌웨어별 표시와 스트림 요청, 파라미터 규칙 데이터를 제공해요.
 * 입력: 모드 조회의 customMode·mavType과 파라미터 이름·기체 종류·버전이에요.
 * 출력: 공식 펌웨어·모드 이름, 초기 스트림 요청 목록, 파라미터 값 방식·메타데이터 출처·묶음 규칙이에요.
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
/** 파라미터 값을 float 칸에 담는 방식을 돌려줘요.
 * 입력: 없어요.
 * 출력: PX4는 Bytewise, ArduPilot과 범용 플러그인은 CCast예요.
 * 실패: 없어요.
 */
    virtual ParamEncoding paramEncoding() const = 0;
/** 파라미터 메타데이터의 출처를 돌려줘요.
 * 입력: HEARTBEAT의 기체 종류 mavType과 major.minor.patch 꼴의 version이에요.
 * 출력: 주소와 캐시 이름이에요. 출처가 없으면 format이 None이에요.
 * 실패: 모르는 기체 종류나 읽을 수 없는 버전은 None이에요.
 */
    virtual ParamMetadataSource paramMetadataSource(int mavType, const QString& version) const = 0;
/** 파라미터가 속할 묶음 이름을 정해요.
 * 입력: 파라미터 이름 name과 메타데이터의 묶음 metadataGroup이에요.
 * 출력: 묶음 이름이며, 정할 수 없으면 빈 문자열이에요.
 * 실패: 없어요.
 */
    virtual QString paramGroup(const QString& name, const QString& metadataGroup) const = 0;
/** 묶음의 사람이 읽는 제목을 돌려줘요.
 * 입력: 묶음 이름 group이에요.
 * 출력: 현재 언어의 제목이며, 제목이 없으면 빈 문자열이에요.
 * 실패: 없어요.
 */
    virtual QString paramGroupTitle(const QString& group) const = 0;
/** 파일 비교에서 건너뛸 파라미터 이름을 돌려줘요.
 * 입력: 없어요.
 * 출력: 기체 고유 값이나 계수처럼 파일에서 올리면 안 되는 이름 목록이에요.
 * 실패: 없어요.
 */
    virtual QStringList paramFileSkip() const = 0;
};
}
