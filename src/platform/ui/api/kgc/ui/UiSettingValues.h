// [0.0.2 T2–T9 T16 T20] specs/features/0.0.2-theme.md
// [0.0.2 T24] specs/features/0.0.2-theme.md
// [0.0.4 T18 T27] specs/features/0.0.4-righttabs.md
#pragma once
#include <QObject>
#include <QSet>
#include <QStringList>
#include <QUrl>
namespace kgc::ui {
    /** 공유 화면 설정을 화면에 노출해요.
     * 입력: 없음.
     * 출력: 없음.
     * 실패: 없음.
     */
class UiSettingValues : public QObject {
    Q_OBJECT
    Q_PROPERTY(int holdToConfirmMs READ holdToConfirmMs WRITE setHoldToConfirmMs NOTIFY holdToConfirmMsChanged)
    Q_PROPERTY(int holdToConfirmMsMin READ holdToConfirmMsMin CONSTANT)
    Q_PROPERTY(int holdToConfirmMsMax READ holdToConfirmMsMax CONSTANT)
    Q_PROPERTY(QString backgroundKind READ backgroundKind WRITE setBackgroundKind NOTIFY backgroundKindChanged)
    Q_PROPERTY(QString backgroundImage READ backgroundImage WRITE setBackgroundImage NOTIFY backgroundImageChanged)
    Q_PROPERTY(QStringList backgroundKinds READ backgroundKinds CONSTANT)
    Q_PROPERTY(QUrl backgroundImageSource READ backgroundImageSource NOTIFY backgroundImageChanged)
    Q_PROPERTY(QString instrumentStyle READ instrumentStyle WRITE setInstrumentStyle NOTIFY instrumentStyleChanged)
    Q_PROPERTY(QStringList instrumentStyles READ instrumentStyles CONSTANT)
public:
    /** 설정 변경을 구독해요.
     * 입력: 부모 객체.
     * 출력: 없음.
     * 실패: 없음.
     */
    explicit UiSettingValues(QObject* parent = nullptr);
    /** 길게 누르는 시간을 읽어요.
     * 입력: 없음.
     * 출력: 밀리초.
     * 실패: 없음.
     */
    int holdToConfirmMs() const;
    /** 허용하는 최소 시간을 읽어요.
     * 입력: 없음.
     * 출력: 밀리초.
     * 실패: 없음.
     */
    int holdToConfirmMsMin() const;
    /** 허용하는 최대 시간을 읽어요.
     * 입력: 없음.
     * 출력: 밀리초.
     * 실패: 없음.
     */
    int holdToConfirmMsMax() const;
    /** 길게 누르는 시간을 저장해요.
     * 입력: 밀리초.
     * 출력: 없음. QML 속성 쓰기이므로 저장 결과를 반환하지 않아요.
     * 실패: 생성 접근자가 false로 거부한 범위 밖 값은 저장하지 않고 기존 값과 변경 시그널을 유지해요.
     * 흐름:
     * 1. settings::setHoldToConfirmMs — 메타데이터 범위를 지켜 시간을 저장해요.
     */
    void setHoldToConfirmMs(int value);
    /** 저장된 배경 종류를 읽어요.
     * 입력: 없음.
     * 출력: 배경 종류 식별자.
     * 실패: 없음.
     */
    QString backgroundKind() const;
    /** 저장된 사진 경로를 읽어요.
     * 입력: 없음.
     * 출력: 사진 파일의 절대 경로.
     * 실패: 없음.
     */
    QString backgroundImage() const;
    /** 메타데이터의 배경 종류 목록을 읽어요.
     * 입력: 없음.
     * 출력: 선택 순서대로 나열한 식별자.
     * 실패: 없음.
     */
    QStringList backgroundKinds() const;
    /** 특수 문자가 있는 사진 경로도 화면에서 열 수 있게 변환해요.
     * 입력: 없음.
     * 출력: 로컬 파일 주소, 경로가 비면 빈 주소.
     * 실패: 파일 읽기 실패는 그리는 쪽에서 알려요.
     */
    QUrl backgroundImageSource() const;
    /** 선택한 배경 종류를 저장해요.
     * 입력: 배경 종류 식별자.
     * 출력: 없음.
     * 실패: 목록에 없는 값은 무시해요.
     * 흐름:
     * 1. settings::setBackgroundKind — 메타데이터에 있는 배경 종류를 저장해요.
     */
    void setBackgroundKind(const QString& value);
    /** 입력하거나 선택한 사진의 로컬 경로를 저장해요.
     * 입력: 파일 경로 또는 파일 선택 대화 상자의 로컬 주소.
     * 출력: 없음.
     * 실패: 원격 주소는 무시해요.
     * 흐름:
     * 1. QUrl::toLocalFile — 파일 선택 주소라면 로컬 경로로 바꿔요.
     * 2. settings::setBackgroundImage — 절대 경로를 저장해요.
     */
    void setBackgroundImage(const QString& value);
    /** 선택한 사진을 배경으로 저장해요.
     * 입력: 파일 경로 또는 파일 선택 대화 상자의 로컬 주소.
     * 출력: 없음.
     * 실패: 원격 주소는 경로 저장에서 무시해요.
     * 흐름:
     * 1. setBackgroundImage — 사진 경로를 정규화해서 저장해요.
     * 2. settings::setBackgroundKind — 사진 배경을 선택해요.
     */
    Q_INVOKABLE void chooseBackgroundImage(const QString& path);
    /** 빈 경로를 포함해 사진을 읽을 수 있는지 판단하고 실패를 한 번만 기록해요.
     * 입력: 읽기를 시도한 사진 경로. 현재 사진 배경의 경로만 판단해요.
     * 출력: 없음.
     * 실패: 없음.
     */
    Q_INVOKABLE void reportBackgroundImageMissing(const QString& path);
    /** 저장된 계기 모양을 읽어요.
     * 입력: 없음.
     * 출력: 계기 모양 식별자(round·pfd·hud·model3d).
     * 실패: 저장값이 목록 밖이면 메타데이터 기본값 pfd를 돌려줘요.
     */
    QString instrumentStyle() const;
    /** 메타데이터의 계기 모양 목록을 읽어요.
     * 입력: 없음.
     * 출력: 단추 순서대로 나열한 식별자.
     * 실패: 없음.
     */
    QStringList instrumentStyles() const;
    /** 계기 칸 단추로 고른 계기 모양을 저장해요.
     * 입력: 계기 모양 식별자.
     * 출력: 없음. 저장되면 instrumentStyleChanged가 나가요.
     * 실패: 목록에 없는 값은 무시해요.
     * 흐름:
     * 1. settings::setInstrumentStyle — 메타데이터에 있는 계기 모양을 저장해요.
     */
    void setInstrumentStyle(const QString& value);
signals:
    /** 시간 설정의 변경을 알려요.
     * 입력: 없음.
     * 출력: 없음.
     * 실패: 없음.
     */
    void holdToConfirmMsChanged();
    /** 배경 종류 변경을 알려요.
     * 입력: 없음.
     * 출력: 없음.
     * 실패: 없음.
     */
    void backgroundKindChanged();
    /** 사진 경로와 화면 주소 변경을 알려요.
     * 입력: 없음.
     * 출력: 없음.
     * 실패: 없음.
     */
    void backgroundImageChanged();
    /** 계기 모양 변경을 알려요.
     * 입력: 없음.
     * 출력: 없음.
     * 실패: 없음.
     */
    void instrumentStyleChanged();
private:
    QSet<QString> warnedImagePaths_;
};
}
