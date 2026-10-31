// [0.2.0 T3] specs/features/0.2.0-fcflash.md
#include <kgc/app/net/Http.h>
#include <kgc/app/net/Errors.h>
#include <zlib.h>
namespace kgc::app::net {
kgc::app::errors::Result<QByteArray> gunzip(const QByteArray& bytes)
{
    // WORKER-FILL: spec §5.4 — zlib inflate(gzip 머리, MAX_WBITS + 16)로 풀어요. 스트림이 끝나지 않으면 BadGzip이에요.
    Q_UNUSED(bytes);
    return std::unexpected(make(HttpError::BadGzip));
}
}
