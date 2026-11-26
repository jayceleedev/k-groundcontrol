// [0.5.0 S2] specs/features/0.5.0-map.md
// [0.2.0 T3] specs/features/0.2.0-fcflash.md
#pragma once
namespace kgc::app::net {
inline constexpr int kInflateChunkSize = 16384;
// 최종 응답 상태가 이 값 이상이면 서버가 거절한 것으로 봐요(HttpError::HttpStatus).
inline constexpr int kHttpErrorStatusMin = 400;
}
