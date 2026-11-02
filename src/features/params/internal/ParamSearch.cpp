// [0.3.0 6.8 T6] specs/features/0.3.0-params.md
#include "ParamSearch.h"
namespace kgc::params {
namespace {
struct DigitRun { qsizetype start; qsizetype end; };
DigitRun digitRun(const QString& text, qsizetype from)
{
    auto start = from;
    while (start < text.size() && text[start] == QLatin1Char('0')) ++start;
    auto end = start;
    while (end < text.size() && text[end].isDigit()) ++end;
    return {start, end};
}
}
bool naturalLess(const QString& a, const QString& b)
{
    qsizetype i = 0;
    qsizetype j = 0;
    while (i < a.size() && j < b.size()) {
        if (a[i].isDigit() && b[j].isDigit()) {
            const auto first = digitRun(a, i);
            const auto second = digitRun(b, j);
            const auto length = first.end - first.start;
            if (length != second.end - second.start) return length < second.end - second.start;
            const auto order = QStringView(a).sliced(first.start, length).compare(QStringView(b).sliced(second.start, length));
            if (order != 0) return order < 0;
            i = first.end;
            j = second.end;
        } else {
            if (a[i] != b[j]) return a[i] < b[j];
            ++i;
            ++j;
        }
    }
    if (a.size() - i != b.size() - j) return a.size() - i < b.size() - j;
    // 앞의 0만 다른 이름도 순서가 하나로 정해지도록 글자 순서로 마무리해요.
    return a < b;
}
QStringList searchTerms(const QString& text) { return text.split(QLatin1Char(' '), Qt::SkipEmptyParts); }
bool matchesSearch(const QStringList& terms, const QString& name, const QString& title, const QString& description)
{
    for (const auto& term : terms) {
        if (!name.contains(term, Qt::CaseInsensitive) && !title.contains(term, Qt::CaseInsensitive)
            && !description.contains(term, Qt::CaseInsensitive)) return false;
    }
    return true;
}
}
