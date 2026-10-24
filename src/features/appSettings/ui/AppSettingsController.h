// [0.0.1 T4–T10] specs/features/0.0.1-shell.md
#pragma once
#include <QObject>
#include <QStringList>
namespace kgc::appSettings {
class AppSettingsService;
/** 설정 화면에 언어와 배포 자료를 제공해요.
 * 입력: 없음.
 * 출력: 없음.
 * 실패: 없음.
 */
class AppSettingsController : public QObject {
    Q_OBJECT
    Q_PROPERTY(QString version READ version CONSTANT)
    Q_PROPERTY(QString sourceUrl READ sourceUrl CONSTANT)
    Q_PROPERTY(QString licenseText READ licenseText CONSTANT)
    Q_PROPERTY(QString noticesText READ noticesText CONSTANT)
    Q_PROPERTY(QStringList languages READ languages CONSTANT)
    Q_PROPERTY(QString language READ language NOTIFY languageChanged)
public:
/** 앱 수명의 서비스에 화면을 연결해요.
 * 입력: 앱 설정 서비스.
 * 출력: 없음.
 * 실패: 없음.
 */
    explicit AppSettingsController(AppSettingsService& service);
/** 앱 버전을 읽어요.
 * 입력: 없음.
 * 출력: 버전.
 * 실패: 없음.
 */
    QString version() const;
/** 소스 위치를 읽어요.
 * 입력: 없음.
 * 출력: 소스 주소.
 * 실패: 없음.
 */
    QString sourceUrl() const;
/** 동봉한 라이선스를 읽어요.
 * 입력: 없음.
 * 출력: 라이선스 전문.
 * 실패: 파일을 못 읽으면 빈 문자열.
 */
    QString licenseText() const;
/** 동봉한 고지를 읽어요.
 * 입력: 없음.
 * 출력: 고지 전문.
 * 실패: 파일을 못 읽으면 빈 문자열.
 */
    QString noticesText() const;
/** 메타데이터의 언어 목록을 읽어요.
 * 입력: 없음.
 * 출력: 언어 코드 목록.
 * 실패: 없음.
 */
    QStringList languages() const;
/** 현재 언어 설정을 읽어요.
 * 입력: 없음.
 * 출력: 언어 코드.
 * 실패: 없음.
 */
    QString language() const;
/** 언어 이름을 그 언어로 표시해요.
 * 입력: 언어 코드.
 * 출력: 언어 이름.
 * 실패: 없음.
 */
    Q_INVOKABLE QString languageName(const QString& code) const;
/** 선택한 언어를 저장해요.
 * 입력: 언어 코드.
 * 출력: 없음.
 * 실패: 없음.
 * 흐름:
 * 1. settings::setLanguage — 선택을 저장해 서비스의 변경 구독에 전달해요.
 */
    Q_INVOKABLE void setLanguage(const QString& code);
signals:
/** 언어 설정 변경을 알려요.
 * 입력: 없음.
 * 출력: 없음.
 * 실패: 없음.
 */
    void languageChanged();
private:
    AppSettingsService& service_;
};
}
