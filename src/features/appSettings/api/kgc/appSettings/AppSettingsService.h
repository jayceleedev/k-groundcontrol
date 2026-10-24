// [0.0.1 T4–T10] specs/features/0.0.1-shell.md
#pragma once
#include <kgc/appSettings/AppInfo.h>
#include <QObject>
namespace kgc::app::i18n { class Translator; }
namespace kgc::appSettings {
/** 언어 설정을 번역기에 적용하고 배포 정보를 제공해요.
 * 입력: 없음.
 * 출력: 없음.
 * 실패: 없음.
 */
class AppSettingsService : public QObject {
    Q_OBJECT
public:
/** 언어 설정의 변경을 구독해요.
 * 입력: 배포 정보와 번역기와 부모.
 * 출력: 없음.
 * 실패: 없음.
 * 흐름:
 * 1. settings::onLanguageChanged — 언어 변경을 구독해요.
 * 2. Translator::setLanguage — 바뀐 언어를 적용해요.
 * 3. KGC_LOG_INFO — 적용 언어의 변경을 기록해요.
 */
    AppSettingsService(AppInfo info, kgc::app::i18n::Translator& translator, QObject* parent = nullptr);
/** 조립이 준 배포 정보를 읽어요.
 * 입력: 없음.
 * 출력: 배포 정보 참조.
 * 실패: 없음.
 */
    const AppInfo& info() const;
private:
    AppInfo info_;
};
}
