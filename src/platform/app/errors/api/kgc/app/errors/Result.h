// [0.0.1 T1] specs/features/0.0.1-shell.md
#pragma once
#include <expected>
#include <kgc/app/errors/ErrorCode.h>

namespace kgc::app::errors {
template<class T> using Result = std::expected<T, ErrorCode>;
}
