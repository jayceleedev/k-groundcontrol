// [0.2.0 T4] specs/features/0.2.0-fcflash.md
#pragma once
#include <kgc/app/errors/Result.h>
#include <QObject>
#include <QString>
#include <memory>
namespace kgc::comm::rawport {
/** 시리얼 포트마다 지금 누가 주인인지 기록해요.
 * 입력: GUI 스레드에서 부르는 점유와 해제예요. Composition이 하나 만들어 LinkManager에 넘겨요.
 * 출력: 포트별 주인과, 해제에 쓰는 토큰이에요.
 * 실패: 이미 주인이 있는 포트의 점유는 PortError::Busy예요.
 */
class PortRegistry : public QObject {
    Q_OBJECT
public:
/** 포트의 주인 종류를 나타내요.
 * 입력: 없어요.
 * 출력: None(없음), Link(MAVLink 링크), Lease(임대)예요.
 * 실패: 없어요.
 */
    enum class Owner { None, Link, Lease };
/** 점유 하나를 가리켜요.
 * 입력: claim이 만들어요.
 * 출력: release에 넘기는 값이에요. 기본값은 아무것도 가리키지 않아요.
 * 실패: 없어요.
 */
    struct Token {
        quint64 id = 0;
        constexpr bool operator==(const Token&) const = default;
    };
/** 빈 등록부를 만들어요.
 * 입력: 수명을 함께할 parent예요.
 * 출력: 주인이 없는 등록부예요.
 * 실패: 없어요.
 */
    explicit PortRegistry(QObject* parent = nullptr);
    ~PortRegistry() override;
/** 포트의 주인이 돼요.
 * 입력: 포트 이름 portName과 주인 종류 owner예요.
 * 출력: 해제에 쓸 Token이에요.
 * 실패: 이미 주인이 있으면 PortError::Busy예요.
 */
    kgc::app::errors::Result<Token> claim(QString portName, Owner owner);
/** 점유를 놓아요.
 * 입력: claim이 돌려준 token이에요.
 * 출력: 그 포트의 주인을 None으로 되돌려요.
 * 실패: 이미 놓았거나 모르는 토큰이면 아무 일도 하지 않아요.
 */
    void release(Token token);
/** 포트의 주인을 읽어요.
 * 입력: 포트 이름 portName이에요.
 * 출력: 현재 Owner이며, 기록이 없으면 None이에요.
 * 실패: 없어요.
 */
    Owner ownerOf(QString portName) const;
private:
    struct Private;
    std::unique_ptr<Private> d_;
};
}
