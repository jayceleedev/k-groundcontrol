// [0.2.0 7] specs/features/0.2.0-fcflash.md
#include <kgc/app/job/Errors.h>
#include <QCoreApplication>
namespace kgc::app::job {
QString errorText(kgc::app::errors::ErrorCode error)
{
    if (error.module != kModuleId) return {};
    switch (static_cast<JobError>(error.code)) {
    case JobError::Cancelled: return QCoreApplication::translate("Job", QT_TRANSLATE_NOOP("Job", "취소했어요."));
    }
    return {};
}
}
