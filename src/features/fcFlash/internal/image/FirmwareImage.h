// [0.2.0 T8] specs/features/0.2.0-fcflash.md
#pragma once
#include <kgc/app/errors/Result.h>
#include <QByteArray>
#include <QString>
namespace kgc::fcFlash {
/** 펌웨어 파일의 종류를 나타내요.
 * 입력: 없어요.
 * 출력: magic이 PX4FWv1이면 Px4, APJFWv1이면 ArduPilot이에요.
 * 실패: 없어요.
 */
enum class FirmwareKind { Px4, ArduPilot };
/** 해석한 펌웨어 파일이에요.
 * 입력: parseFirmwareImage가 채워요.
 * 출력: 종류, 파일의 보드 ID, 풀린 이미지와 크기, summary, 비교 버전이에요.
 *       expectedVersion은 PX4 파일이면 git_identity의 버전, ArduPilot 파일이면 빈 값이에요.
 * 실패: 없어요.
 */
struct FirmwareImage {
    FirmwareKind kind = FirmwareKind::Px4;
    int boardId = 0;
    QByteArray image;
    QByteArray extfImage;
    QString summary;
    QString expectedVersion;
    bool operator==(const FirmwareImage&) const = default;
};
/** .px4·.apj 파일의 JSON을 해석해요.
 * 입력: 파일 내용 json이에요.
 * 출력: image·extf_image를 base64 + zlib로 푼 FirmwareImage예요. extf_* 키가 없으면 외부 이미지는 비어요.
 * 실패: JSON이 아니거나, magic을 모르거나, 풀 수 없거나, 풀린 길이가 image_size·extf_image_size와 다르면 BadImage예요.
 */
kgc::app::errors::Result<FirmwareImage> parseFirmwareImage(const QByteArray& json);
/** 글자에서 x.y.z 꼴의 첫 버전을 찾아요.
 * 입력: git_identity나 태그 같은 text예요.
 * 출력: \d+\.\d+\.\d+의 첫 일치이며, 없으면 빈 값이에요.
 * 실패: 없어요.
 */
QString versionInText(const QString& text);
/** 보드와 파일의 보드 ID가 호환되는지 판정해요.
 * 입력: 부트로더의 boardId와 파일의 fileBoardId예요.
 * 출력: 같으면 참이에요. 9와 255는 같은 계열이라 서로 참이에요.
 * 실패: 없어요.
 */
bool compatible(int boardId, int fileBoardId);
/** 이미지를 이 보드에 구워도 되는지 봐요.
 * 입력: image, 보드 ID boardId, 플래시 크기 flashSize, 외부 플래시 크기 extfSize예요.
 * 출력: 구워도 되면 성공이에요.
 * 실패: 보드가 다르면 WrongBoard, 내부·외부 이미지가 플래시보다 크면 TooLarge예요.
 */
kgc::app::errors::Result<void> checkImage(const FirmwareImage& image, int boardId, qint64 flashSize, qint64 extfSize);
}
