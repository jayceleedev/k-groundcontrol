// [0.4.0 5.4 6.2 6.4 6.9] specs/features/0.4.0-sitl.md
#include "SitlDownload.h"
#include "Constants.h"
#include "Events.h"
#include <kgc/sitl/Errors.h>
#include <kgc/app/job/Errors.h>
#include <kgc/app/job/Job.h>
#include <kgc/app/log/Log.h>
#include <kgc/app/net/Errors.h>
#include <kgc/app/net/Http.h>
#include <QDir>
#include <QFile>
#include <QRandomGenerator>
namespace kgc::sitl {
namespace {
// 로그는 영어만 쓰므로 Http 오류는 열거 이름으로, 다른 모듈의 코드는 번호로 적어요.
QString downloadErrorName(kgc::app::errors::ErrorCode error)
{
    using kgc::app::net::HttpError;
    if (error.module == kgc::app::net::kModuleId) {
        switch (static_cast<HttpError>(error.code)) {
        case HttpError::NotHttps: return QStringLiteral("NotHttps");
        case HttpError::RedirectRefused: return QStringLiteral("RedirectRefused");
        case HttpError::NetworkFailed: return QStringLiteral("NetworkFailed");
        case HttpError::TimedOut: return QStringLiteral("TimedOut");
        case HttpError::WriteFailed: return QStringLiteral("WriteFailed");
        case HttpError::Aborted: return QStringLiteral("Aborted");
        case HttpError::BadGzip: return QStringLiteral("BadGzip");
        }
    }
    return QStringLiteral("%1:%2").arg(error.module.value).arg(error.code);
}
}
SitlDownload::SitlDownload(kgc::app::net::Http& http, QList<SitlFile> files, QString cacheRoot, QString folderName,
                           bool executable, QObject* parent)
    : QObject(parent), http_(http), files_(std::move(files)), cacheRoot_(std::move(cacheRoot)),
      folderName_(std::move(folderName)), executable_(executable),
      job_(new kgc::app::job::Job(kgc::app::job::OperationKind::Read, this))
{
    connect(job_, &kgc::app::job::Job::cancelRequested, this, &SitlDownload::cancel);
}
SitlDownload::~SitlDownload()
{
    if (request_) {
        request_->disconnect(this);
        request_->abort();
    }
    if (!done_ && !partDir_.isEmpty()) QDir(partDir_).removeRecursively();
}
kgc::app::job::Job* SitlDownload::job() const { return job_; }
void SitlDownload::start()
{
    KGC_LOG_INFO(kCategory, SitlEvent::SitlDownloadStarted, "SITL download started: %1", folderName_);
    job_->setStep(QT_TRANSLATE_NOOP("Job", "내려받기"));
    job_->setCancellable(true);
    job_->setProgress(0);
    // 받는 중의 파일은 캐시 폴더 옆 .part-<무작위> 폴더에 두어 기존 캐시를 그대로 둬요.
    const auto random = QString::number(QRandomGenerator::global()->generate(), 16);
    partDir_ = QDir(cacheRoot_).filePath(QString::fromLatin1(kPartPrefix) + random);
    if (cacheRoot_.isEmpty() || !QDir().mkpath(partDir_)) {
        finishWith(QStringLiteral("CreateFolderFailed"));
        return;
    }
    next();
}
void SitlDownload::next()
{
    if (index_ >= files_.size()) { complete(); return; }
    const auto& file = files_[index_];
    request_ = http_.download(file.url, QDir(partDir_).filePath(file.fileName));
    const auto count = double(files_.size());
    connect(request_, &kgc::app::net::HttpRequest::progress, this, [this, count](qint64 received, qint64 total) {
        const double part = total > 0 ? double(received) / double(total) : 0.0;
        job_->setProgress((index_ + qBound(0.0, part, 1.0)) / count);
    });
    connect(request_, &kgc::app::net::HttpRequest::finished, this, &SitlDownload::fileFinished);
}
void SitlDownload::fileFinished(const kgc::app::errors::Result<QByteArray>& result)
{
    request_ = nullptr;
    if (!result) {
        finishWith(downloadErrorName(result.error()));
        return;
    }
    ++index_;
    job_->setProgress(double(index_) / double(files_.size()));
    next();
}
void SitlDownload::complete()
{
    if (executable_) {
        // 첫 파일이 실행 파일이에요. 소유자 rwx, 그룹·기타 rx를 줘요.
        const auto path = QDir(partDir_).filePath(files_.first().fileName);
        const auto permissions = QFile::ReadOwner | QFile::WriteOwner | QFile::ExeOwner | QFile::ReadGroup | QFile::ExeGroup | QFile::ReadOther | QFile::ExeOther;
        if (!QFile::setPermissions(path, permissions)) {
            finishWith(QStringLiteral("PermissionFailed"));
            return;
        }
    }
    const auto target = QDir(cacheRoot_).filePath(folderName_);
    if ((QDir(target).exists() && !QDir(target).removeRecursively()) || !QDir().rename(partDir_, target)) {
        finishWith(QStringLiteral("RenameFailed"));
        return;
    }
    done_ = true;
    KGC_LOG_INFO(kCategory, SitlEvent::SitlDownloadFinished, "SITL download finished: %1", folderName_);
    job_->succeed();
    emit finished({});
}
void SitlDownload::cancel()
{
    if (done_) return;
    done_ = true;
    if (request_) {
        request_->disconnect(this);
        request_->abort();
        request_ = nullptr;
    }
    QDir(partDir_).removeRecursively();
    job_->finishCancelled();
    emit finished(std::unexpected(make(kgc::app::job::JobError::Cancelled)));
}
void SitlDownload::finishWith(const QString& reason)
{
    done_ = true;
    if (!partDir_.isEmpty()) QDir(partDir_).removeRecursively();
    KGC_LOG_WARN(kCategory, SitlEvent::SitlDownloadFailed, "SITL download failed: %1, %2", folderName_, reason);
    const auto failure = make(SitlError::DownloadFailed);
    job_->fail(failure, errorText(failure));
    emit finished(std::unexpected(failure));
}
}
