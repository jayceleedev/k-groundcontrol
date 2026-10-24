// [0.0.1 T1] specs/features/0.0.1-shell.md
#pragma once
#include <QStringList>

namespace kgc::app::boot {
/** 부팅에 필요한 값을 묶어서 전달해요.
 * 입력: 없음.
 * 출력: 없음.
 * 실패: 없음.
 */
struct BootOptions {
    QString logLevel;
    int maxFileMB;
    int maxFiles;
    QString dataDir;
/** 명령줄을 부팅 파일보다 우선해서 읽어요.
 * 입력: 프로그램 이름을 첫 항목으로 둔 인자 목록.
 * 출력: 부팅 값 묶음.
 * 실패: 잘못된 명령줄은 사용법을 출력하고 종료.
 */
    static BootOptions parse(const QStringList& args);
};
}
