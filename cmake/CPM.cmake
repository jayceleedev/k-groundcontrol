# [0.0.1 T1] specs/features/0.0.1-shell.md
# [0.0.2 T23] specs/features/0.0.2-theme.md
# 의존성 스크립트를 빌드 폴더에 받고 고정한 해시로 검증해요.
include_guard(GLOBAL)
set(CPM_DOWNLOAD_VERSION 0.40.8)
set(CPM_DOWNLOAD_SHA256 78ba32abdf798bc616bab7c73aac32a17bbd7b06ad9e26a6add69de8f3ae4791)

# 의존성과 내려받기 도구 모두 빌드 폴더 안에서 해시로 확인해요.
set(CPM_SOURCE_CACHE "${PROJECT_BINARY_DIR}/cpm-cache" CACHE PATH "의존성 원본 캐시")
set(cpm_script "${PROJECT_BINARY_DIR}/cmake/CPM_${CPM_DOWNLOAD_VERSION}.cmake")
if(NOT EXISTS "${cpm_script}")
    file(DOWNLOAD
        "https://github.com/cpm-cmake/CPM.cmake/releases/download/v${CPM_DOWNLOAD_VERSION}/CPM.cmake"
        "${cpm_script}" EXPECTED_HASH "SHA256=${CPM_DOWNLOAD_SHA256}" TLS_VERIFY ON)
endif()
include("${cpm_script}")
