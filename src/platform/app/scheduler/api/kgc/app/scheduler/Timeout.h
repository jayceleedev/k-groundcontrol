// [0.2.0 T6] specs/features/0.2.0-fcflash.md
#pragma once
#include <QObject>
#include <QString>
#include <chrono>
#include <memory>
namespace kgc::app::scheduler {
/** 이름 붙은 일회성 시간 재기를 맡아요.
 * 입력: 로그에만 쓰는 이름과 부모 객체예요.
 * 출력: 시간이 다 되면 expired 시그널을 한 번 내요.
 * 실패: 없어요.
 */
class Timeout : public QObject {
    Q_OBJECT
public:
/** 멈춘 시간 재기를 만들어요.
 * 입력: 로그에만 쓰는 name과 수명을 함께할 parent예요.
 * 출력: start 전까지 아무 일도 하지 않는 객체예요.
 * 실패: 없어요.
 */
    Timeout(QString name, QObject* parent);
/** 멈춰 있지 않은 시간 재기도 함께 지워요.
 * 입력: 없어요.
 * 출력: 지워진 뒤 expired는 나오지 않아요.
 * 실패: 없어요.
 */
    ~Timeout() override;
/** 시간 재기를 처음부터 시작해요.
 * 입력: 기다릴 duration이에요.
 * 출력: duration 뒤 expired를 한 번 내요. 이미 재는 중이면 처음부터 다시 재요.
 * 실패: 없어요.
 */
    void start(std::chrono::milliseconds duration);
/** 시간 재기를 멈춰요.
 * 입력: 없어요.
 * 출력: 다시 start할 때까지 expired가 나오지 않아요.
 * 실패: 재는 중이 아니면 아무 일도 하지 않아요.
 */
    void stop();
signals:
/** 시간이 다 되었음을 알려요.
 * 입력: 없어요.
 * 출력: start마다 많아야 한 번 나와요.
 * 실패: 없어요.
 */
    void expired();
private:
    struct Private;
    std::unique_ptr<Private> d_;
};
}
