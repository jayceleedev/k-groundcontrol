// [0.5.0 S2 T8] specs/features/0.5.0-map.md
// [0.2.0 5.5] specs/features/0.2.0-fcflash.md
// [0.1.0 T8] specs/features/0.1.0-link.md
// [0.0.1 T1] specs/features/0.0.1-shell.md
#pragma once
#include <kgc/app/log/LogLevel.h>
#include <QStringList>
#include <optional>

namespace kgc::app::boot {
/** 부팅에 필요한 값을 묶어서 전달해요.
 * 입력: 없음.
 * 출력: 없음.
 * 실패: 없음.
 */
struct BootOptions {
    kgc::app::log::LogLevel logLevel;
    int maxFileMB;
    int maxFiles;
    QString dataDir;
    QStringList extraSerial;
    // 시험용 타일 서버 주소(parseTileHost를 거친 값)와 그 서버 인증서 PEM 파일이에요. 명령줄에서만 읽고 비면 쓰지 않아요.
    QString tileHost;
    QString tileCa;
/** 명령줄을 부팅 파일보다 우선해서 읽어요.
 * 입력: 프로그램 이름을 첫 항목으로 둔 인자 목록.
 * 출력: 부팅 값 묶음이며 반복한 --extra-serial은 경로 목록으로 모아요. 파일의 로그 수준이 잘못되면 Info를 사용해요.
 *       --tile-host·--tile-ca는 명령줄에서만 읽고 kgc.ini에는 두지 않아요.
 * 실패: 잘못된 명령줄이나 명령줄 로그 수준, https://호스트[:포트] 꼴이 아닌 --tile-host는 오류를 출력하고 종료.
 */
    static BootOptions parse(const QStringList& args);
/** 시험용 타일 서버 주소를 검사하고 다듬어요.
 * 입력: 명령줄의 --tile-host 값 value.
 * 출력: https이고 호스트가 있으며 경로가 비었거나 "/"이고 질의·조각·사용자 정보가 없으면 끝 "/" 없는 주소(호스트 소문자).
 * 실패: 그 밖은 값 없음.
 */
    static std::optional<QString> parseTileHost(const QString& value);
/** 시험용 인증서를 읽지 못했음을 터미널에 알리고 끝내요.
 * 입력: --tile-ca로 받은 경로 path.
 * 출력: 없음(종료 코드 1로 프로그램을 끝내요).
 * 실패: 없음.
 */
    [[noreturn]] static void exitBadCertificate(const QString& path);
};
}
