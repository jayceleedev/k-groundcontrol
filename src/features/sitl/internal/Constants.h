// [0.4.0 6.2 6.3 6.5 6.6] specs/features/0.4.0-sitl.md
#pragma once
#include <chrono>
namespace kgc::sitl {
// 내려받을 주소의 바탕이에요(스펙 §6.2).
inline constexpr auto kWindowsBase = "https://firmware.ardupilot.org/Tools/MissionPlanner/sitl/";
inline constexpr auto kWindowsCopterStable = "CopterStable/";
inline constexpr auto kWindowsPlaneStable = "PlaneStable/";
inline constexpr auto kWindowsRoverStable = "RoverStable/";
inline constexpr auto kWindowsBeta = "Beta/";
inline constexpr auto kWindowsExecutableSource = ".elf";
inline constexpr auto kWindowsExecutable = ".exe";
// Windows 실행 파일 옆에 같은 이름으로 두는 Cygwin DLL 10개예요.
inline constexpr const char* kWindowsDlls[] = {
    "cygatomic-1.dll", "cyggcc_s-1.dll", "cyggcc_s-seh-1.dll", "cyggomp-1.dll", "cygiconv-2.dll",
    "cygintl-8.dll", "cygquadmath-0.dll", "cygssp-0.dll", "cygstdc++-6.dll", "cygwin1.dll"
};
inline constexpr auto kLinuxBase = "https://firmware.ardupilot.org/";
inline constexpr auto kLinuxFolder = "SITL_x86_64_linux_gnu";
inline constexpr auto kLinuxHeliFolder = "SITL_x86_64_linux_gnu-heli";
inline constexpr auto kParamsBase = "https://raw.githubusercontent.com/ArduPilot/ardupilot/master/Tools/autotest/";
inline constexpr auto kCopterParams = "default_params/copter.parm";
inline constexpr auto kHeliParams = "default_params/copter-heli.parm";
inline constexpr auto kPlaneParams = "models/plane.parm";
inline constexpr auto kRoverParams = "default_params/rover.parm";
inline constexpr auto kDefaultsFile = "defaults.parm";
// 명령줄 값이에요(스펙 §6.3).
inline constexpr auto kUdpClientHost = "udpclient:127.0.0.1:";
inline constexpr int kLatLonDecimals = 7;
inline constexpr int kAltitudeDecimals = 1;
// 내려받는 중의 폴더 이름 앞붙이예요(스펙 §6.4 2단계).
inline constexpr auto kPartPrefix = ".part-";
// 프로세스와 출력의 시간·양이에요(스펙 §5.4·§6.5).
inline constexpr std::chrono::milliseconds kSitlStopWait{3000};
inline constexpr int kSitlOutputLines = 200;
inline constexpr std::chrono::milliseconds kSitlOutputThrottle{200};
}
