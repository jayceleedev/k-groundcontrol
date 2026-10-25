// [0.0.1 T1] specs/features/0.0.1-shell.md
#pragma once
#include <QObject>
#include <QSettings>
#include <QJsonArray>

namespace kgc::app::settings {
/** 메타데이터에 등록된 운영 설정을 저장해요.
 * 입력: 없음.
 * 출력: 없음.
 * 실패: 없음.
 */
class Settings : public QObject {
    Q_OBJECT
public:
/** 앱 수명의 저장소를 얻어요.
 * 입력: 없음.
 * 출력: 설정 저장소 참조.
 * 실패: 없음.
 */
    static Settings& instance();
/** 저장값 또는 등록된 기본값을 읽어요.
 * 입력: 설정 식별자.
 * 출력: 설정 값.
 * 실패: 미등록 식별자는 빈 값.
 */
    QVariant value(const QString& id) const;
/** 설정을 저장하고 변경을 알려요.
 * 입력: 설정 식별자와 값.
 * 출력: 저장을 허용하면 참.
 * 실패: 미등록 식별자, 잘못된 열거 값, 범위 밖 숫자는 거짓.
 */
    bool setValue(const QString& id, const QVariant& value);
/** 전체 설정 정의를 읽어요.
 * 입력: 없음.
 * 출력: 생성기가 모은 메타데이터.
 * 실패: 없음.
 */
    QJsonArray definitions() const;
signals:
/** 바뀐 설정의 식별자를 알려요.
 * 입력: 설정 식별자.
 * 출력: 없음.
 * 실패: 없음.
 */
    void changed(const QString& id);
private:
    Settings();
    static QJsonArray registry();
    QSettings storage_;
    QJsonArray definitions_;
};
}
