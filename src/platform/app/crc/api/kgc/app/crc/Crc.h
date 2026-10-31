// [0.2.0 T1] specs/features/0.2.0-fcflash.md
#pragma once
#include <kgc/app/crc/Constants.h>
#include <QByteArrayView>
#include <cstdint>
namespace kgc::app::crc {
/** PX4·ArduPilot 부트로더가 쓰는 CRC-32를 계산해요.
 * 입력: 계산할 bytes와 이어서 계산할 state예요. state를 넘기면 앞 조각의 결과에 이어 붙여 셀 수 있어요.
 * 출력: 다항식 0xEDB88320, 초기값 0, 마지막 XOR 없는 결과예요. zlib의 crc32와는 값이 달라요.
 * 실패: 없어요.
 */
inline std::uint32_t crc32Bootloader(QByteArrayView bytes, std::uint32_t state = kBootloaderCrcInitial)
{
    // WORKER-FILL: spec §5.1 — 반사형 표 또는 비트 단위로 계산해요. 초기값 0, 마지막 XOR 없음.
    (void)bytes;
    return state;
}
}
