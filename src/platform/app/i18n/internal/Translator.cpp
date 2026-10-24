// [0.0.1 T1] specs/features/0.0.1-shell.md
#include <kgc/app/i18n/Translator.h>
#include <QCoreApplication>
#include <QLocale>
namespace kgc::app::i18n {
Translator::Translator(QString qmDir, QObject* parent)
    : QObject(parent), qmDir_(std::move(qmDir)) {}
QString Translator::language() const { return language_; }
void Translator::setLanguage(const QString& code)
{
    QString language = code == "system" ? QLocale::system().name().section('_', 0, 0) : code;
    if (language != "ko" && language != "en") language = "en";
    if (language == language_) return;
    QCoreApplication::removeTranslator(&translator_);
    if (language == "en" && translator_.load(qmDir_ + "/kgc_en.qm"))
        QCoreApplication::installTranslator(&translator_);
    language_ = language;
    emit languageChanged();
}
}
