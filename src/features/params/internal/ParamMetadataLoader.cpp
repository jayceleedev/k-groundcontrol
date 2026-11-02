// [0.3.0 6.3] specs/features/0.3.0-params.md
#include "ParamMetadataLoader.h"
#include "ParamMetadata.h"
#include <kgc/app/net/Http.h>
#include <kgc/app/paths/Paths.h>
#include <QFile>
namespace kgc::params {
using kgc::vehicle::firmware::ParamMetadataFormat;
ParamMetadataLoader::ParamMetadataLoader(kgc::app::net::Http& http, QObject* parent) : QObject(parent), http_(http) {}
void ParamMetadataLoader::start(const kgc::vehicle::firmware::ParamMetadataSource& source)
{
    source_ = source;
    next_ = 0;
    path_ = kgc::app::paths::Paths::cacheMetadata() + QStringLiteral("/params-%1.json").arg(source.cacheName);
    auto cached = parseFile();
    if (!cached.isEmpty()) {
        emit loaded(cached);
        return;
    }
    tryNext();
}
QHash<QString, ParamMeta> ParamMetadataLoader::parseFile() const
{
    QFile file(path_);
    if (!file.open(QIODevice::ReadOnly)) return {};
    const auto bytes = file.readAll();
    return source_.format == ParamMetadataFormat::Flat ? parseFlatMetadata(bytes) : parseGroupedMetadata(bytes);
}
void ParamMetadataLoader::tryNext()
{
    if (next_ >= source_.urls.size()) {
        emit failed();
        return;
    }
    auto* request = http_.download(source_.urls[next_++], path_);
    connect(request, &kgc::app::net::HttpRequest::finished, this, [this](const auto& result) {
        if (!result) {
            tryNext();
            return;
        }
        auto metadata = parseFile();
        if (metadata.isEmpty()) emit failed();
        else emit loaded(metadata);
    });
}
}
