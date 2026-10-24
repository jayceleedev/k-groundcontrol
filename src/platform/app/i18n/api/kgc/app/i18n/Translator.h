// [0.0.1 T1] specs/features/0.0.1-shell.md
#pragma once
#include <QObject>
#include <QTranslator>

namespace kgc::app::i18n {
/** 지원 언어에 맞는 번역기를 관리해요.
 * 입력: 없음.
 * 출력: 없음.
 * 실패: 없음.
 */
class Translator : public QObject {
    Q_OBJECT
    Q_PROPERTY(QString language READ language NOTIFY languageChanged)
public:
/** 번역 파일의 기준 경로를 받아요.
 * 입력: 번역 파일 폴더와 부모.
 * 출력: 없음.
 * 실패: 없음.
 */
    explicit Translator(QString qmDir, QObject* parent = nullptr);
/** 적용된 언어를 읽어요.
 * 입력: 없음.
 * 출력: 언어 코드.
 * 실패: 없음.
 */
    QString language() const;
/** 설정 코드를 실제 지원 언어로 적용해요.
 * 입력: system 또는 언어 코드.
 * 출력: 없음.
 * 실패: 번역 파일을 못 읽으면 원문 유지.
 */
    void setLanguage(const QString& code);
signals:
/** 적용 언어 변경을 알려요.
 * 입력: 없음.
 * 출력: 없음.
 * 실패: 없음.
 */
    void languageChanged();
private:
    QString qmDir_;
    QString language_;
    QTranslator translator_;
};
}
