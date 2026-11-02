// [0.3.0 5.5 6.8-1] specs/features/0.3.0-params.md
#pragma once
#include <QObject>
#include <QtQml/qqmlregistration.h>
namespace kgc::params {
Q_NAMESPACE
QML_NAMED_ELEMENT(ParamStates)
/** 파라미터 내려받기의 지금 상태를 나타내요.
 * 입력: 없어요.
 * 출력: Idle·Downloading·Complete·Incomplete·SkippedArmed 중 하나예요.
 * 실패: 없어요.
 */
enum class DownloadState { Idle, Downloading, Complete, Incomplete, SkippedArmed };
Q_ENUM_NS(DownloadState)
/** 파라미터 메타데이터의 지금 상태를 나타내요.
 * 입력: 없어요.
 * 출력: None·Loading·Ready·Failed 중 하나예요.
 * 실패: 없어요.
 */
enum class MetadataState { None, Loading, Ready, Failed };
Q_ENUM_NS(MetadataState)
}
