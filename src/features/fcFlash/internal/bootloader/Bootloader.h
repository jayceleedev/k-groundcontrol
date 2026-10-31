// [0.2.0 T7] specs/features/0.2.0-fcflash.md
#pragma once
#include <kgc/app/errors/Result.h>
#include <kgc/fcFlash/Errors.h>
#include <kgc/comm/rawport/ByteStream.h>
#include "image/FirmwareImage.h"
#include <QByteArray>
#include <atomic>
#include <functional>
namespace kgc::fcFlash {
/** 부트로더가 알려 준 보드 정보예요.
 * 입력: identify가 채워요.
 * 출력: 부트로더 판, 보드 ID, 보드 판, 플래시 크기(바이트), 외부 플래시 크기(바이트, 없으면 0)예요.
 * 실패: 없어요.
 */
struct BoardInfo {
    int bootloaderRev = 0;
    int boardId = 0;
    int boardRev = 0;
    qint64 flashSize = 0;
    qint64 extfSize = 0;
};
/** 굽는 중 지금 하는 단계를 나타내요.
 * 입력: 없어요.
 * 출력: Erase·Program·Verify 중 하나이며, 외부 플래시도 같은 값을 써요.
 * 실패: 없어요.
 */
enum class BootloaderStage { Erase, Program, Verify };
/** ByteStream 위에서 PX4·ArduPilot 부트로더와 대화해요.
 * 입력: 작업 스레드에서 쓰는 stream과 중단 플래그 abort예요.
 * 출력: 단계마다 Result예요. 읽기는 100ms 조각으로 나눠 조각마다 중단 플래그를 봐요.
 * 실패: 중단은 Aborted, 포트 유실은 PortLost, 그 밖은 단계별 오류 코드예요.
 */
class Bootloader {
public:
    using ProgressFn = std::function<void(double)>;
    using StageFn = std::function<void(BootloaderStage)>;
/** 대화 상대를 정해요.
 * 입력: 수명 동안 유지되는 stream과 abort예요.
 * 출력: 아직 아무것도 보내지 않은 객체예요.
 * 실패: 없어요.
 */
    Bootloader(kgc::comm::rawport::ByteStream& stream, const std::atomic_bool& abort);
/** 부트로더가 있는지 확인해요.
 * 입력: 없어요.
 * 출력: 0x21 0x20을 보내 0x12 0x10을 받으면 성공이에요. 300ms씩 세 번 해 봐요.
 * 실패: 세 번 다 응답이 없으면 BootloaderNotFound예요.
 */
    kgc::app::errors::Result<void> sync();
/** 보드 정보를 읽어요.
 * 입력: 없어요.
 * 출력: GET_DEVICE 1, 2, 3, 4와, 판이 5 이상이면 6을 읽은 BoardInfo예요. 6이 실패하면 외부 플래시는 0이에요.
 *       값을 기억해 두고 verify와 flashImage가 써요.
 * 실패: 판이 3~20 밖이면 BadBootloaderRevision, 응답이 없으면 BootloaderNotFound예요.
 */
    kgc::app::errors::Result<BoardInfo> identify();
/** 내부 플래시를 지워요.
 * 입력: 경과 시간에 비례한 0~1을 받을 progress예요.
 * 출력: 0x23 0x20을 보내고 응답을 받으면 성공이에요. 시간 제한은 20초, 플래시가 2000 KB를 넘으면 1 MB마다 4초 더해요.
 * 실패: 응답이 없거나 실패 응답이면 EraseFailed예요.
 */
    kgc::app::errors::Result<void> erase(const ProgressFn& progress);
/** 내부 플래시에 써요.
 * 입력: 이미지 image와 쓴 비율 0~1을 받을 progress예요.
 * 출력: 0x27 len data 0x20을 252바이트 묶음으로 보내요. 이미지는 4바이트 배수로 0xFF를 덧붙여요.
 * 실패: 묶음 하나라도 실패하면 재시도 없이 ProgramFailed예요.
 */
    kgc::app::errors::Result<void> program(const QByteArray& image, const ProgressFn& progress);
/** 내부 플래시의 CRC를 확인해요.
 * 입력: 쓴 이미지 image예요.
 * 출력: 0x29 0x20의 응답이 image와 0xFF 채움을 identify의 플래시 크기까지 셈한 값과 같으면 성공이에요. 시간 제한은 5초예요.
 * 실패: 다르거나 응답이 없으면 VerifyFailed예요.
 */
    kgc::app::errors::Result<void> verify(const QByteArray& image);
/** 외부 플래시를 지워요.
 * 입력: 지울 크기 size와 0~1을 받을 progress예요.
 * 출력: 0x34 <u32 LE size> 0x20을 보내요. 부트로더는 동기 응답, 퍼센트 바이트들, 100, 마지막 동기 응답을 줘요.
 *       퍼센트 18은 0x12와 같으니 동기 응답으로 읽지 않아요.
 * 실패: 응답이 없거나 실패 응답이면 EraseFailed예요.
 */
    kgc::app::errors::Result<void> extfErase(qint64 size, const ProgressFn& progress);
/** 외부 플래시에 써요.
 * 입력: 외부 이미지 image와 progress예요.
 * 출력: 0x35 len data 0x20을 252바이트 묶음으로 보내요. 이미지는 4바이트 배수로 0xFF를 덧붙여요.
 * 실패: 묶음 하나라도 실패하면 재시도 없이 ProgramFailed예요.
 */
    kgc::app::errors::Result<void> extfProgram(const QByteArray& image, const ProgressFn& progress);
/** 외부 플래시의 CRC를 확인해요.
 * 입력: 외부 이미지 image예요.
 * 출력: 0x37 <u32 LE size> 0x20의 응답이 image 길이만큼, 채움 없이 셈한 값과 같으면 성공이에요. 시간 제한은 10초예요.
 * 실패: 다르거나 응답이 없으면 VerifyFailed예요.
 */
    kgc::app::errors::Result<void> extfVerify(const QByteArray& image);
/** 이미지 하나를 스펙 §6.4 순서로 구워요.
 * 입력: 해석한 image, 단계 알림 stage, 단계 안 진행률 progress예요. identify 뒤에 불러요.
 * 출력: 외부 이미지가 있으면 외부부터 지우기·쓰기·검증을 하고, image_size가 0이 아니면 내부를 해요. boot는 부르지 않아요.
 * 실패: 외부 플래시가 모자라면 굽기 전에 TooLarge예요. 그 밖은 실패한 단계의 오류예요.
 */
    kgc::app::errors::Result<void> flashImage(const FirmwareImage& image, const StageFn& stage, const ProgressFn& progress);
/** FC를 부팅시켜요.
 * 입력: 없어요.
 * 출력: 0x30 0x20을 보내고 응답을 기다리지 않아요. 검증이 성공했을 때와 취소하고 부팅할 때만 불러요.
 * 실패: 포트를 잃으면 PortLost예요.
 */
    kgc::app::errors::Result<void> boot();
private:
    kgc::app::errors::Result<void> send(const QByteArray& bytes, FcFlashError failure);
    kgc::app::errors::Result<QByteArray> receive(int count, std::chrono::milliseconds timeout,
        FcFlashError failure, const ProgressFn& progress = {});
    kgc::app::errors::Result<QByteArray> reply(int count, std::chrono::milliseconds timeout,
        FcFlashError failure, const ProgressFn& progress = {});
    kgc::app::errors::Result<void> programChunks(char command, const QByteArray& image, const ProgressFn& progress);
    kgc::comm::rawport::ByteStream& stream_;
    const std::atomic_bool& abort_;
    BoardInfo board_;
};
}
