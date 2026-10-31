// [0.2.0 S3 S4] specs/features/0.2.0-fcflash.md
// [0.1.0 T1–T6] specs/features/0.1.0-link.md
#pragma once
#include <kgc/comm/links/LinkManager.h>
#include <kgc/comm/links/Link.h>
#include <QPointer>
#include <QThread>
#include <kgc/comm/rawport/PortRegistry.h>
namespace kgc::comm::links {
struct LinkManager::Private {
    QThread* thread = nullptr;
    QPointer<Link> worker;
    LinkConfig config;
    QList<QPair<QString, QString>> ports;
    QStringList extraSerial;
    QPointer<kgc::comm::rawport::PortRegistry> registry;
    kgc::comm::rawport::PortRegistry::Token token;
    int nextId = 0;
    int id = -1;
    bool open = false;
    bool failed = false;
    bool closing = false;
    bool cancelled = false;
    std::optional<kgc::app::errors::ErrorCode> error;
};
}
