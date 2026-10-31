// [0.2.0 T7–T10 T14–T22] specs/features/0.2.0-fcflash.md
#pragma once
#include <QtGlobal>
#include <chrono>
#include <cstdint>
namespace kgc::fcFlash {
// 서비스의 대기 시간이에요(스펙 §6.9).
inline constexpr std::chrono::milliseconds kHeartbeatWait{3000};
inline constexpr std::chrono::milliseconds kRebootWait{8000};
inline constexpr std::chrono::milliseconds kBootloaderWait{30000};
inline constexpr std::chrono::milliseconds kReconnectWait{60000};
inline constexpr std::chrono::milliseconds kVersionWait{5000};
inline constexpr std::chrono::milliseconds kScanPeriod{500};
inline constexpr std::chrono::milliseconds kPortSettle{500};
inline constexpr std::chrono::milliseconds kReturnGrace{3000};
inline constexpr int kFlashBaud = 115200;
inline constexpr float kRebootToBootloader = 3.0F;
// 부트로더 대화의 값이에요(조사 §Q1·§Q4).
inline constexpr char kEoc = 0x20;
inline constexpr char kInSync = 0x12;
inline constexpr char kOk = 0x10;
inline constexpr char kFailed = 0x11;
inline constexpr char kInvalid = 0x13;
inline constexpr char kGetSync = 0x21;
inline constexpr char kGetDevice = 0x22;
inline constexpr char kChipErase = 0x23;
inline constexpr char kProgMulti = 0x27;
inline constexpr char kGetCrc = 0x29;
inline constexpr char kReboot = 0x30;
inline constexpr char kExtfErase = 0x34;
inline constexpr char kExtfProgMulti = 0x35;
inline constexpr char kExtfGetCrc = 0x37;
inline constexpr char kInfoBootloaderRev = 1;
inline constexpr char kInfoBoardId = 2;
inline constexpr char kInfoBoardRev = 3;
inline constexpr char kInfoFlashSize = 4;
inline constexpr char kInfoExtfSize = 6;
inline constexpr int kMinBootloaderRev = 3;
inline constexpr int kMaxBootloaderRev = 20;
inline constexpr int kMinExtfBootloaderRev = 5;
inline constexpr int kSyncTries = 3;
inline constexpr std::chrono::milliseconds kSyncTimeout{300};
inline constexpr std::chrono::milliseconds kReadSlice{100};
inline constexpr std::chrono::milliseconds kEraseTimeout{20000};
inline constexpr std::chrono::milliseconds kEraseExtraPerMb{4000};
inline constexpr qint64 kEraseExtraAboveBytes = 2000 * 1024;
inline constexpr std::chrono::milliseconds kVerifyTimeout{5000};
inline constexpr std::chrono::milliseconds kExtfVerifyTimeout{10000};
inline constexpr int kProgramChunk = 252;
inline constexpr char kPadByte = static_cast<char>(0xFF);
// 파일 형식과 보드 ID의 값이에요(스펙 §6.5·§6.6).
inline constexpr auto kMagicArduPilot = "APJFWv1";
inline constexpr auto kMagicPx4 = "PX4FWv1";
inline constexpr int kBoardFmuV2 = 9;
inline constexpr int kBoardFmuV3 = 255;
inline constexpr auto kPx4ReleasesUrl = "https://api.github.com/repos/PX4/PX4-Autopilot/releases?per_page=30";
inline constexpr auto kArduPilotManifestUrl = "https://firmware.ardupilot.org/manifest.json.gz";
inline constexpr auto kPx4BoardsResource = ":/kgc/data/fcflash/px4-boards.json";
inline constexpr auto kBootloaderLabel = "bootloader";
}
