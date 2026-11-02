// [0.3.0 6.8 6.8-1] specs/features/0.3.0-params.md
#pragma once
#include <kgc/params/ParamService.h>
#include <kgc/vehicle/registry/VehicleRegistry.h>
#include <QPointer>
#include <QQmlEngine>
#include <QUrl>
#include <QVariantList>
#include <QVariantMap>
class QAbstractItemModel;
namespace kgc::params {
class ParamsFilterModel;
/** 활성 기체의 파라미터를 QML에 옮기고 서비스를 불러요.
 * 입력: 기체 등록부이며, activeChanged마다 ParamService::of(active)에 다시 묶어요.
 * 출력: 표 모델과 표시값, 토스트 글자예요. 판정은 서비스가 맡아요.
 * 실패: 실패는 toast로 알려요.
 * 흐름(쓰기):
 * 1. writeCheck — ParamService::writeDecision으로 쓸 수 있는지 봐요.
 * 2. write — ParamService::writePending을 불러요.
 * 3. ParamService::writeFinished — 결과 토스트를 내요.
 */
class ParamsController : public QObject {
    Q_OBJECT
    QML_ELEMENT
    QML_SINGLETON
    Q_PROPERTY(bool hasVehicle READ hasVehicle NOTIFY stateChanged)
    Q_PROPERTY(QAbstractItemModel* model READ model CONSTANT)
    Q_PROPERTY(int visibleCount READ visibleCount NOTIFY visibleCountChanged)
    Q_PROPERTY(int paramCount READ paramCount NOTIFY paramCountChanged)
    Q_PROPERTY(int pendingCount READ pendingCount NOTIFY pendingChanged)
    Q_PROPERTY(bool hasDefaults READ hasDefaults NOTIFY stateChanged)
    Q_PROPERTY(kgc::params::DownloadState downloadState READ downloadState NOTIFY stateChanged)
    Q_PROPERTY(kgc::params::MetadataState metadataState READ metadataState NOTIFY stateChanged)
    Q_PROPERTY(kgc::app::job::Job* job READ job NOTIFY stateChanged)
    Q_PROPERTY(bool rebootNeeded READ rebootNeeded NOTIFY stateChanged)
    Q_PROPERTY(int receivedCount READ receivedCount NOTIFY stateChanged)
    Q_PROPERTY(int totalCount READ totalCount NOTIFY stateChanged)
    Q_PROPERTY(QVariantList groups READ groups NOTIFY groupsChanged)
    Q_PROPERTY(QString groupKey READ groupKey WRITE setGroupKey NOTIFY filterChanged)
    Q_PROPERTY(QString searchText READ searchText WRITE setSearchText NOTIFY filterChanged)
    Q_PROPERTY(bool pendingOnly READ pendingOnly WRITE setPendingOnly NOTIFY filterChanged)
    Q_PROPERTY(bool nonDefaultOnly READ nonDefaultOnly WRITE setNonDefaultOnly NOTIFY filterChanged)
    Q_PROPERTY(QString selectedName READ selectedName WRITE setSelectedName NOTIFY selectedChanged)
    Q_PROPERTY(QVariantMap selected READ selected NOTIFY selectedChanged)
    Q_PROPERTY(QVariantList diffRows READ diffRows NOTIFY diffChanged)
    Q_PROPERTY(QString diffSummary READ diffSummary NOTIFY diffChanged)
    Q_PROPERTY(QUrl saveFolder READ saveFolder NOTIFY stateChanged)
    Q_PROPERTY(QUrl saveFileUrl READ saveFileUrl NOTIFY stateChanged)
public:
    /** 입력: 기체 등록부예요. 출력: 등록부가 소유하는 컨트롤러예요. 실패: 중복 생성은 프로그래밍 오류예요. */
    explicit ParamsController(kgc::vehicle::registry::VehicleRegistry& vehicles);
    /** 입력: QML 엔진과 스크립트 엔진이에요. 출력: C++가 소유하는 싱글턴이에요. 실패: 등록 전 호출은 프로그래밍 오류예요. */
    static ParamsController* create(QQmlEngine* engine, QJSEngine* scriptEngine);
    /** 입력: 없어요. 출력: 활성 기체의 서비스가 있으면 참이에요. 실패: 없어요. */
    bool hasVehicle() const;
    /** 입력: 없어요. 출력: 걸러진 표 모델이에요. 실패: 없어요. */
    QAbstractItemModel* model() const;
    /** 입력: 없어요. 출력: 보이는 행 수예요. 실패: 없어요. */
    int visibleCount() const;
    /** 입력: 없어요. 출력: 전체 행 수예요. 실패: 없어요. */
    int paramCount() const;
    /** 입력: 없어요. 출력: 대기값 수예요. 실패: 없어요. */
    int pendingCount() const;
    /** 입력: 없어요. 출력: 메타데이터에 기본값이 있으면 참이에요. 실패: 없어요. */
    bool hasDefaults() const;
    /** 입력: 없어요. 출력: 내려받기 상태예요. 실패: 없어요. */
    DownloadState downloadState() const;
    /** 입력: 없어요. 출력: 메타데이터 상태예요. 실패: 없어요. */
    MetadataState metadataState() const;
    /** 입력: 없어요. 출력: 진행 중 Job이며 없으면 nullptr예요. 실패: 없어요. */
    kgc::app::job::Job* job() const;
    /** 입력: 없어요. 출력: 재부팅이 필요하면 참이에요. 실패: 없어요. */
    bool rebootNeeded() const;
    /** 입력: 없어요. 출력: 받은 수예요. 실패: 없어요. */
    int receivedCount() const;
    /** 입력: 없어요. 출력: 전체 수예요. 실패: 없어요. */
    int totalCount() const;
    /** 입력: 없어요. 출력: 전체·묶음·기타 순서의 {key, count} 목록이에요. 실패: 없어요. */
    QVariantList groups() const;
    /** 입력: 없어요. 출력: 고른 묶음 키예요. 실패: 없어요. */
    QString groupKey() const;
    /** 입력: 묶음 키 key예요. 출력: 표를 다시 걸러요. 실패: 없어요. */
    void setGroupKey(const QString& key);
    /** 입력: 없어요. 출력: 검색어예요. 실패: 없어요. */
    QString searchText() const;
    /** 입력: 검색어 text예요. 출력: 표를 다시 걸러요. 실패: 없어요. */
    void setSearchText(const QString& text);
    /** 입력: 없어요. 출력: 대기값 토글이에요. 실패: 없어요. */
    bool pendingOnly() const;
    /** 입력: 토글 값 value예요. 출력: 표를 다시 걸러요. 실패: 없어요. */
    void setPendingOnly(bool value);
    /** 입력: 없어요. 출력: 기본값과 다른 값 토글이에요. 실패: 없어요. */
    bool nonDefaultOnly() const;
    /** 입력: 토글 값 value예요. 출력: 표를 다시 걸러요. 실패: 없어요. */
    void setNonDefaultOnly(bool value);
    /** 입력: 없어요. 출력: 고른 행의 이름이에요. 실패: 없어요. */
    QString selectedName() const;
    /** 입력: 행 이름 name이에요. 출력: 고른 행을 바꿔요. 실패: 없어요. */
    void setSelectedName(const QString& name);
    /** 입력: 없어요. 출력: 고른 행의 표시값이며 없으면 빈 맵이에요. 실패: 없어요. */
    QVariantMap selected() const;
    /** 입력: 없어요. 출력: 비교 표의 {name, current, file} 목록이에요. 실패: 없어요. */
    QVariantList diffRows() const;
    /** 입력: 없어요. 출력: 비교 창의 수치 한 줄이에요. 실패: 없어요. */
    QString diffSummary() const;
    /** 입력: 없어요. 출력: 저장 대화상자의 기본 폴더예요. 실패: 없어요. */
    QUrl saveFolder() const;
    /** 입력: 없어요. 출력: 저장 대화상자의 기본 파일이에요. 실패: 없어요. */
    QUrl saveFileUrl() const;
    /** 묶음 줄의 글자를 만들어요. 입력: 묶음 key예요. 출력: "제목 (묶음)" 또는 묶음 이름이에요. 실패: 없어요. */
    Q_INVOKABLE QString groupLabel(const QString& key) const;
    /** 입력한 값을 검사해요.
     * 입력: 파라미터 이름 name과 글자 text예요.
     * 출력: {unchanged, error, ask, range, value}이에요. ask가 참이면 범위 밖이라 물어봐야 해요.
     * 실패: error 글자로 알려요.
     */
    Q_INVOKABLE QVariantMap checkValue(const QString& name, const QString& text) const;
    /** 대기값을 정해요.
     * 입력: 이름 name과 값 value예요.
     * 출력: 표가 갱신돼요.
     * 실패: toast로 알려요.
     * 흐름:
     * 1. ParamService::setPending — 대기값을 정해요.
     */
    Q_INVOKABLE void setPending(const QString& name, double value);
    /** 대기값을 지워요. 입력: 이름 name이에요. 출력: 표가 갱신돼요. 실패: 없어요. */
    Q_INVOKABLE void revert(const QString& name);
    /** 대기값을 기본값으로 정해요. 입력: 이름 name이에요. 출력: 표가 갱신돼요. 실패: toast로 알려요. */
    Q_INVOKABLE void restoreDefault(const QString& name);
    /** 선택지 편집기의 목록을 만들어요.
     * 입력: 이름 name이에요.
     * 출력: {items: [{value, text}], current}이에요. 지금 값이 목록에 없으면 맨 앞에 숫자 항목이 있어요.
     * 실패: 없어요.
     */
    Q_INVOKABLE QVariantMap optionModel(const QString& name) const;
    /** 비트 창의 목록을 만들어요. 입력: 이름 name이에요. 출력: [{bit, label, checked}]예요. 실패: 없어요. */
    Q_INVOKABLE QVariantList bitModel(const QString& name) const;
    /** 비트 창의 결과 값을 만들어요.
     * 입력: 이름 name과 켠 비트 번호 chosen이에요.
     * 출력: 지금 보이는 값에서 목록의 비트만 바꾼 값이에요. 목록에 없는 비트는 그대로예요.
     * 실패: 없는 이름은 0이에요.
     */
    Q_INVOKABLE double bitsValue(const QString& name, const QList<int>& chosen) const;
    /** 쓰기 확인 창의 표를 만들어요. 입력: 없어요. 출력: [{name, current, next}]예요. 실패: 없어요. */
    Q_INVOKABLE QVariantList pendingRows() const;
    /** 쓸 수 있는지 봐요.
     * 입력: 없어요.
     * 출력: "allow"·"warn"·"deny"예요. deny면 토스트도 내요.
     * 실패: 없어요.
     * 흐름:
     * 1. ParamService::writeDecision — 시동 중 규칙을 판정해요.
     */
    Q_INVOKABLE QString writeCheck();
    /** 대기값을 써요.
     * 입력: 없어요.
     * 출력: 끝나면 토스트를 내요.
     * 실패: 토스트로 알려요.
     * 흐름:
     * 1. ParamService::writePending — 백업 뒤 대기값을 써요.
     */
    Q_INVOKABLE void write();
    /** 파라미터를 다시 받아요.
     * 입력: 없어요.
     * 출력: Job이 시작돼요.
     * 실패: 토스트로 알려요.
     * 흐름:
     * 1. ParamService::refresh — 내려받기를 시작해요.
     */
    Q_INVOKABLE void refresh();
    /** FC를 재부팅해요.
     * 입력: 없어요.
     * 출력: 결과 토스트를 내요.
     * 실패: 토스트로 알려요.
     * 흐름:
     * 1. ParamService::reboot — 재부팅 명령을 보내요.
     */
    Q_INVOKABLE void reboot();
    /** 값을 파일로 저장해요.
     * 입력: 파일 주소 url이에요.
     * 출력: 결과 토스트를 내요.
     * 실패: 토스트로 알려요.
     * 흐름:
     * 1. ParamService::saveFile — FC 값을 파일로 써요.
     */
    Q_INVOKABLE void saveFile(const QUrl& url);
    /** 파일을 읽어 비교해요.
     * 입력: 파일 주소 url이에요.
     * 출력: 성공하면 참이고 diffRows·diffSummary가 바뀌어요.
     * 실패: 토스트로 알리고 거짓을 돌려줘요.
     * 흐름:
     * 1. ParamService::compareFile — 파일과 FC 값을 비교해요.
     */
    Q_INVOKABLE bool openFile(const QUrl& url);
    /** 고른 파일 값을 대기값으로 올려요.
     * 입력: 고른 이름 names예요.
     * 출력: 표가 갱신돼요.
     * 실패: 없어요.
     * 흐름:
     * 1. ParamService::applyFile — 고른 값을 대기값으로 정해요.
     */
    Q_INVOKABLE void applyFile(const QStringList& names);
signals:
    /** 입력: 없어요. 출력: 서비스 표시값을 다시 읽도록 알려요. 실패: 없어요. */
    void stateChanged();
    /** 입력: 없어요. 출력: 보이는 행 수를 다시 읽도록 알려요. 실패: 없어요. */
    void visibleCountChanged();
    /** 입력: 없어요. 출력: 전체 행 수를 다시 읽도록 알려요. 실패: 없어요. */
    void paramCountChanged();
    /** 입력: 없어요. 출력: 대기값 수를 다시 읽도록 알려요. 실패: 없어요. */
    void pendingChanged();
    /** 입력: 없어요. 출력: 묶음 목록을 다시 읽도록 알려요. 실패: 없어요. */
    void groupsChanged();
    /** 입력: 없어요. 출력: 거름 조건을 다시 읽도록 알려요. 실패: 없어요. */
    void filterChanged();
    /** 입력: 없어요. 출력: 고른 행을 다시 읽도록 알려요. 실패: 없어요. */
    void selectedChanged();
    /** 입력: 없어요. 출력: 비교 결과를 다시 읽도록 알려요. 실패: 없어요. */
    void diffChanged();
    /** 입력: 보일 글자 text예요. 출력: 패널이 토스트로 보여요. 실패: 없어요. */
    void toast(const QString& text);
private:
    void rebind();
    void watchJob();
    void bindService();
    void writeDone(int written, const QStringList& failed);
    const Param* row(const QString& name) const;
    const ParamMeta* meta(const QString& name) const;
    kgc::vehicle::registry::VehicleRegistry& vehicles_;
    ParamsFilterModel* filter_;
    QPointer<kgc::vehicle::registry::Vehicle> vehicle_;
    QPointer<ParamService> service_;
    QString groupKey_ = QStringLiteral("*");
    QString searchText_;
    QString selectedName_;
    bool pendingOnly_ = false;
    bool nonDefaultOnly_ = false;
    FileDiff diff_;
    int lastWritten_ = 0;
    inline static QPointer<ParamsController> instance_;
};
}
