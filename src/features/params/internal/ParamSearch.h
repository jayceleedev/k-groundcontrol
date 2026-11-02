// [0.3.0 6.8 T6] specs/features/0.3.0-params.md
#pragma once
#include <QString>
#include <QStringList>
namespace kgc::params {
/** 이름을 사람이 기대하는 순서로 견줘요.
 * 입력: 이름 a·b예요.
 * 출력: a가 앞이면 참이에요. 숫자 구간은 수로 견줘서 RC2_가 RC10_보다 앞이에요.
 * 실패: 없어요.
 */
bool naturalLess(const QString& a, const QString& b);
/** 검색어를 낱말로 나눠요.
 * 입력: 사용자가 넣은 글자 text예요.
 * 출력: 공백으로 나눈 낱말 목록이에요.
 * 실패: 없어요.
 */
QStringList searchTerms(const QString& text);
/** 행이 검색어에 맞는지 판정해요.
 * 입력: 낱말 terms와 이름·제목·설명이에요.
 * 출력: 낱말 전부가 셋 중 하나에 대소문자 없이 들어 있으면 참이에요.
 * 실패: 없어요.
 */
bool matchesSearch(const QStringList& terms, const QString& name, const QString& title, const QString& description);
}
