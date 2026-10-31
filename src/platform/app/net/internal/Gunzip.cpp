// [0.2.0 T3] specs/features/0.2.0-fcflash.md
#include <kgc/app/net/Http.h>
#include <kgc/app/net/Errors.h>
#include "Constants.h"
#include <zlib.h>
#include <array>
#include <limits>
namespace kgc::app::net {
kgc::app::errors::Result<QByteArray> gunzip(const QByteArray& bytes)
{
    if (bytes.size() > std::numeric_limits<uInt>::max()) return std::unexpected(make(HttpError::BadGzip));
    z_stream stream{};
    stream.next_in = reinterpret_cast<Bytef*>(const_cast<char*>(bytes.constData()));
    stream.avail_in = static_cast<uInt>(bytes.size());
    if (inflateInit2(&stream, MAX_WBITS + 16) != Z_OK) return std::unexpected(make(HttpError::BadGzip));
    QByteArray output;
    std::array<char, kInflateChunkSize> buffer;
    int status;
    do {
        stream.next_out = reinterpret_cast<Bytef*>(buffer.data());
        stream.avail_out = buffer.size();
        status = inflate(&stream, Z_NO_FLUSH);
        output.append(buffer.data(), buffer.size() - stream.avail_out);
    } while (status == Z_OK);
    inflateEnd(&stream);
    if (status != Z_STREAM_END) return std::unexpected(make(HttpError::BadGzip));
    return output;
}
}
