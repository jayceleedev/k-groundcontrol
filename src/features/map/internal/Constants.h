// [0.5.1 5.2] specs/features/0.5.1-map-sharp.md
// [0.5.0 5.6.4] specs/features/0.5.0-map.md
#pragma once
#include <QtGlobal>
#include <array>
#include <chrono>
namespace kgc::map {
// 지도 확대 범위예요.
inline constexpr int kMinZoom = 2;
inline constexpr int kMaxZoom = 20;
// 타일 한 칸의 크기(논리 px)예요. 2배 타일 그림(512 px)도 같은 칸에 그려요.
inline constexpr int kTileSize = 256;
// 웹 메르카토르가 그리는 위도의 끝과 확대 0 적도에서 256 px 타일의 1 px 길이(m)예요.
inline constexpr double kMaxMercatorLatitude = 85.05112878;
inline constexpr double kMetersPerPixelAtZoom0 = 156543.03392804097;
// 크기 글자와 저장 한도(MB → 바이트)에 쓰는 단위예요.
inline constexpr qint64 kBytesPerKilobyte = 1024;
// 영역 받기 한 번의 타일 수 상한이에요.
inline constexpr qint64 kMaxDownloadTiles = 100000;
// 저장소가 이만큼 저장할 때마다 본 타일을 한도에 맞춰 지워요.
inline constexpr int kEvictEverySaves = 200;
// 지도 종류(settings::MapType 순서)마다 타일 한 장의 평균 크기(바이트)로, 받기 전 크기 어림에 써요.
inline constexpr std::array<qint64, 3> kAvgTileBytes{15 * 1024, 20 * 1024, 10 * 1024};
// 영역 받기에서 동시에 기다리는 타일 수예요.
inline constexpr int kDownloadParallel = 4;
// 지도가 멈춘 뒤 본 곳을 저장하기까지 기다리는 시간(ms)이에요.
inline constexpr int kSaveViewDelayMs = 2000;
// 타일 요청의 무응답 시간과, 서버에 닿지 않는 동안 다시 물어보는 간격이에요.
inline constexpr std::chrono::milliseconds kTileTimeout = std::chrono::seconds(10);
inline constexpr std::chrono::milliseconds kOnlineProbeInterval = std::chrono::seconds(10);
// Qt Location 캐시의 메모리 크기와 화면 밖 텍스처 여유분(바이트)이에요.
inline constexpr int kTileMemoryCacheBytes = 3 * 1024 * 1024;
inline constexpr int kTileExtraTextureBytes = 6 * 1024 * 1024;
// register.cpp가 QML 엔진에 타일 공급을 두는 동적 프로퍼티 이름이에요.
inline constexpr char kTileSourceProperty[] = "kgcTileSource";
// 마우스 휠 한 칸의 angleDelta 값이에요. 모은 휠 값이 이만큼마다 한 층이에요.
inline constexpr int kWheelNotch = 120;
// 잔 값 흐름(정밀 터치패드·부드러운 휠)에서 한 층을 넘긴 뒤 다음 층까지 기다리는 시간(ms)이에요.
inline constexpr qint64 kWheelStepIntervalMs = 130;
// 휠이 이만큼 쉬면 한 몸짓이 끝난 것으로 보고 휠 모음을 비워요.
inline constexpr std::chrono::milliseconds kWheelGestureTimeout = std::chrono::milliseconds(250);
// 핀치가 이만큼(층) 넘게 움직였는데 반올림이 시작 층이면 움직인 방향으로 한 층 가요.
inline constexpr double kPinchSnapMin = 0.3;
// 핀치가 남긴 14.9999999 같은 확대를 정수 층으로 보는 여유예요.
inline constexpr double kZoomEpsilon = 1e-6;
// 화면 배율이 이 값 이상이면 2배 해상도 타일(512 px)을 받아요.
inline constexpr double kHighDpiMinRatio = 1.15;
// 2배 타일 한 장의 바이트가 1배의 약 몇 배인지예요(영역 받기 크기 어림에 써요).
inline constexpr qint64 kHighDpiBytesFactor = 4;
}
