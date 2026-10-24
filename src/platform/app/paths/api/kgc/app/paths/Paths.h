// [0.0.1 T1] specs/features/0.0.1-shell.md
#pragma once
#include <QString>

namespace kgc::app::paths {
/** 데이터 경로를 한 곳에서 제공해요.
 * 입력: 없음.
 * 출력: 없음.
 * 실패: 없음.
 */
class Paths {
public:
/** 기준 폴더를 정해요.
 * 입력: 데이터 폴더.
 * 출력: 없음.
 * 실패: 없음.
 */
    static void init(const QString& dataDir);
/** 기준 폴더를 읽어요.
 * 입력: 없음.
 * 출력: 절대 경로.
 * 실패: 없음.
 */
    static QString dataDir();
/** 필요한 데이터 폴더를 만들고 경로를 돌려줘요.
 * 입력: 없음.
 * 출력: 폴더 경로.
 * 실패: 만들 수 없으면 빈 문자열.
 */
    static QString logsApp();
/** 필요한 데이터 폴더를 만들고 경로를 돌려줘요.
 * 입력: 없음.
 * 출력: 폴더 경로.
 * 실패: 만들 수 없으면 빈 문자열.
 */
    static QString logsTlog();
/** 필요한 데이터 폴더를 만들고 경로를 돌려줘요.
 * 입력: 없음.
 * 출력: 폴더 경로.
 * 실패: 만들 수 없으면 빈 문자열.
 */
    static QString logsOnboard();
/** 필요한 데이터 폴더를 만들고 경로를 돌려줘요.
 * 입력: 없음.
 * 출력: 폴더 경로.
 * 실패: 만들 수 없으면 빈 문자열.
 */
    static QString missions();
/** 필요한 데이터 폴더를 만들고 경로를 돌려줘요.
 * 입력: 없음.
 * 출력: 폴더 경로.
 * 실패: 만들 수 없으면 빈 문자열.
 */
    static QString layouts();
/** 필요한 데이터 폴더를 만들고 경로를 돌려줘요.
 * 입력: 없음.
 * 출력: 폴더 경로.
 * 실패: 만들 수 없으면 빈 문자열.
 */
    static QString paramsBackups();
/** 필요한 데이터 폴더를 만들고 경로를 돌려줘요.
 * 입력: 없음.
 * 출력: 폴더 경로.
 * 실패: 만들 수 없으면 빈 문자열.
 */
    static QString paramsCache();
/** 필요한 데이터 폴더를 만들고 경로를 돌려줘요.
 * 입력: 없음.
 * 출력: 폴더 경로.
 * 실패: 만들 수 없으면 빈 문자열.
 */
    static QString aiChanges();
/** 필요한 데이터 폴더를 만들고 경로를 돌려줘요.
 * 입력: 없음.
 * 출력: 폴더 경로.
 * 실패: 만들 수 없으면 빈 문자열.
 */
    static QString video();
/** 필요한 데이터 폴더를 만들고 경로를 돌려줘요.
 * 입력: 없음.
 * 출력: 폴더 경로.
 * 실패: 만들 수 없으면 빈 문자열.
 */
    static QString cacheFirmware();
/** 필요한 데이터 폴더를 만들고 경로를 돌려줘요.
 * 입력: 없음.
 * 출력: 폴더 경로.
 * 실패: 만들 수 없으면 빈 문자열.
 */
    static QString cacheTiles();
/** 필요한 데이터 폴더를 만들고 경로를 돌려줘요.
 * 입력: 없음.
 * 출력: 폴더 경로.
 * 실패: 만들 수 없으면 빈 문자열.
 */
    static QString cacheMetadata();
/** 필요한 데이터 폴더를 만들고 경로를 돌려줘요.
 * 입력: 없음.
 * 출력: 폴더 경로.
 * 실패: 만들 수 없으면 빈 문자열.
 */
    static QString cacheTerrain();
private:
    static QString directory(const QString& relative);
};
}
