// [0.2.0 T7] specs/features/0.2.0-fcflash.md
#pragma once
#include <kgc/comm/rawport/ByteStream.h>
#include <kgc/comm/rawport/Errors.h>
#include <QByteArray>
#include <QHash>
#include <QList>
#include <QThread>
#include <chrono>
#include <functional>
/** 시험 안에서 CRC를 따로 셈해요. 제품의 crc32Bootloader에 기대지 않아야 T7이 그 값을 믿지 않고 확인해요. */
inline quint32 referenceCrc(const QByteArray& bytes)
{
    quint32 state = 0;
    for (const auto byte : bytes) {
        state ^= static_cast<quint8>(byte);
        for (int bit = 0; bit < 8; ++bit)
            state = (state >> 1) ^ ((state & 1U) != 0U ? 0xEDB88320U : 0U);
    }
    return state;
}
/** PX4·ArduPilot 부트로더처럼 답하는 가짜 ByteStream이에요.
 * 받은 바이트를 명령으로 풀어 기록하고 응답을 쌓아 둬요. read는 짧게 나눠서 돌려줘
 * 부르는 쪽이 이어 붙이는지도 함께 확인돼요. 시간은 실제로 거의 쓰지 않아요.
 */
class FakeBootStream final : public kgc::comm::rawport::ByteStream {
public:
    enum class Op { Sync, GetDevice, Erase, Program, Crc, Boot, ExtfErase, ExtfProgram, ExtfCrc };
    // 보드가 알려 줄 값이에요.
    quint32 revision = 5;
    quint32 boardId = 1013;
    quint32 boardRevision = 1;
    quint32 flashSize = 0x10000;
    quint32 extfSize = 0;
    // 주입할 동작이에요.
    bool silent = false;              // 어떤 명령에도 답하지 않아요.
    bool extfInfoFails = false;       // GET_DEVICE 6에 INVALID로 답해요.
    int failProgramAt = -1;           // 이 번호(0부터)의 쓰기 묶음에 FAILED로 답해요.
    bool corruptCrc = false;          // 내부 CRC 응답을 틀리게 줘요.
    bool corruptExtfCrc = false;      // 외부 CRC 응답을 틀리게 줘요.
    bool eraseSilent = false;         // 지우기에만 답하지 않아요.
    int holdReads = 0;                // 이만큼의 read는 응답이 있어도 Timeout이에요.
    int loseAfterWrites = -1;         // 이만큼 쓴 뒤 포트를 잃어요.
    std::function<void()> onRead;     // read마다 불려요.
    // 기록이에요.
    QList<Op> ops;
    QByteArray sent;
    QList<int> programSizes;
    QList<int> extfProgramSizes;
    QList<int> deviceArgs;
    QByteArray flash;
    QByteArray extf;
    quint32 extfEraseSize = 0;
    quint32 extfCrcSize = 0;
    int reads = 0;
    std::chrono::milliseconds longestRead{0};

    kgc::app::errors::Result<void> write(QByteArray bytes) override
    {
        if (loseAfterWrites == 0) return lost();
        if (loseAfterWrites > 0) --loseAfterWrites;
        sent += bytes;
        inbox_ += bytes;
        parse();
        return {};
    }
    kgc::app::errors::Result<QByteArray> read(int count, std::chrono::milliseconds timeout) override
    {
        ++reads;
        longestRead = std::max(longestRead, timeout);
        if (onRead) onRead();
        if (holdReads > 0 || pending_.isEmpty()) {
            holdReads = std::max(0, holdReads - 1);
            QThread::msleep(kIdleSleepMs);
            return std::unexpected(kgc::comm::rawport::make(kgc::comm::rawport::PortError::Timeout));
        }
        const auto taken = pending_.first(std::min({count, kReadChunk, static_cast<int>(pending_.size())}));
        pending_.remove(0, taken.size());
        return taken;
    }
    void clearInput() override { pending_.clear(); }
    bool canControlLine() const override { return false; }
    kgc::app::errors::Result<void> setBaud(int) override { return {}; }
    kgc::app::errors::Result<void> setDtrRts(bool, bool) override { return {}; }
    int count(Op op) const { return static_cast<int>(ops.count(op)); }
    bool bootSeen() const { return ops.contains(Op::Boot); }
    /** 시작 인사와 정보 읽기를 뺀, 플래시를 건드리는 명령만 순서대로 돌려줘요. */
    QList<Op> flashOps() const
    {
        QList<Op> result;
        for (const auto op : ops)
            if (op != Op::Sync && op != Op::GetDevice && (result.isEmpty() || result.last() != op)) result << op;
        return result;
    }
    /** 지금까지 쓴 내용과 0xFF 바탕으로 플래시 전체의 CRC를 셈해요. */
    quint32 flashCrc() const
    {
        auto image = flash;
        image.append(QByteArray(static_cast<qsizetype>(flashSize) - image.size(), static_cast<char>(0xFF)));
        return referenceCrc(image);
    }
private:
    static constexpr int kReadChunk = 3;
    static constexpr unsigned long kIdleSleepMs = 1;
    static constexpr char kEoc = 0x20;
    static constexpr char kInSync = 0x12;
    static constexpr char kOk = 0x10;
    static constexpr char kFailed = 0x11;
    static constexpr char kInvalid = 0x13;
    static kgc::app::errors::Result<void> lost()
    {
        return std::unexpected(kgc::comm::rawport::make(kgc::comm::rawport::PortError::Lost));
    }
    static QByteArray word(quint32 value)
    {
        QByteArray bytes;
        for (int shift = 0; shift < 32; shift += 8) bytes.append(static_cast<char>((value >> shift) & 0xFFU));
        return bytes;
    }
    static quint32 wordAt(const QByteArray& bytes, int offset)
    {
        quint32 value = 0;
        for (int index = 3; index >= 0; --index)
            value = (value << 8) | static_cast<quint8>(bytes[offset + index]);
        return value;
    }
    void reply(const QByteArray& bytes = {}, char status = kOk)
    {
        if (silent) return;
        pending_ += bytes;
        pending_.append(kInSync);
        pending_.append(status);
    }
    /** 끝까지 도착한 명령을 하나씩 처리하고, 모자라면 다음 write를 기다려요. */
    void parse()
    {
        while (!inbox_.isEmpty()) {
            const auto length = commandLength();
            if (length == 0 || inbox_.size() < length) return;
            if (inbox_[length - 1] != kEoc) {
                reply({}, kInvalid);
            } else {
                handle(inbox_.first(length));
            }
            inbox_.remove(0, length);
        }
    }
    int commandLength() const
    {
        switch (static_cast<quint8>(inbox_[0])) {
        case 0x21: case 0x23: case 0x29: case 0x30: return 2;
        case 0x22: return 3;
        case 0x34: case 0x37: return 6;
        case 0x27: case 0x35: return inbox_.size() < 2 ? 0 : 3 + static_cast<quint8>(inbox_[1]);
        default: return 1;
        }
    }
    void handle(const QByteArray& command)
    {
        switch (static_cast<quint8>(command[0])) {
        case 0x21: ops << Op::Sync; reply(); break;
        case 0x22: getDevice(static_cast<quint8>(command[1])); break;
        case 0x23: ops << Op::Erase; flash.clear(); if (!eraseSilent) reply(); break;
        case 0x27: program(command); break;
        case 0x29: ops << Op::Crc; reply(word(flashCrc() ^ (corruptCrc ? 1U : 0U))); break;
        case 0x30: ops << Op::Boot; break;
        case 0x34: extfErase(command); break;
        case 0x35: extfProgram(command); break;
        case 0x37: extfCrc(command); break;
        default: reply({}, kInvalid); break;
        }
    }
    void getDevice(int arg)
    {
        ops << Op::GetDevice;
        deviceArgs << arg;
        const QHash<int, quint32> values{{1, revision}, {2, boardId}, {3, boardRevision}, {4, flashSize}, {6, extfSize}};
        if (!values.contains(arg) || (arg == 6 && extfInfoFails)) reply({}, kInvalid);
        else reply(word(values[arg]));
    }
    void program(const QByteArray& command)
    {
        ops << Op::Program;
        const auto data = command.sliced(2, command.size() - 3);
        const auto index = static_cast<int>(programSizes.size());
        programSizes << static_cast<int>(data.size());
        if (index == failProgramAt) { reply({}, kFailed); return; }
        flash += data;
        reply();
    }
    /** 실제 부트로더처럼 동기 응답 뒤에 퍼센트 바이트를 주고 마지막에 다시 동기 응답을 줘요. 18은 0x12와 같아 일부러 넣어요. */
    void extfErase(const QByteArray& command)
    {
        ops << Op::ExtfErase;
        extfEraseSize = wordAt(command, 1);
        extf.clear();
        reply();
        if (silent) return;
        for (const char percent : {0, 0, 18, 50, 90, 100}) pending_.append(percent);
        reply();
    }
    void extfProgram(const QByteArray& command)
    {
        ops << Op::ExtfProgram;
        const auto data = command.sliced(2, command.size() - 3);
        extfProgramSizes << static_cast<int>(data.size());
        extf += data;
        reply();
    }
    void extfCrc(const QByteArray& command)
    {
        ops << Op::ExtfCrc;
        extfCrcSize = wordAt(command, 1);
        reply(word(referenceCrc(extf.first(extfCrcSize)) ^ (corruptExtfCrc ? 1U : 0U)));
    }
    QByteArray inbox_;
    QByteArray pending_;
};
