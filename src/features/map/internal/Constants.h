// [0.5.0 5.6.4] specs/features/0.5.0-map.md
#pragma once
#include <QtGlobal>
#include <array>
#include <chrono>
namespace kgc::map {
// 지도 확대 범위와 타일 한 장의 크기(px)예요.
inline constexpr int kMinZoom = 2;
inline constexpr int kMaxZoom = 20;
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
}
