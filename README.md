# K-GroundControl

GPL-3.0 ground control station for Windows and Linux, built with C++23 and Qt 6.12.0.
Source: https://github.com/jayceleedev/k-groundcontrol

## Build on Linux

Install GCC 13+, CMake 3.28+, Ninja, Qt 6.12.0 (including Quick, QuickControls2,
LinguistTools, Test, QuickTest), and the checker dependencies.

```sh
source ~/.local/share/kgc-tools/venv/bin/activate
export QT_ROOT=~/Qt/6.12.0/gcc_64
python3 -m pip install -r tools/check/requirements.txt
git config core.hooksPath tools/check/hooks
cmake --preset linux-release
cmake --build --preset linux-release
ctest --preset linux-release
python3 tools/check/run.py
LD_LIBRARY_PATH=~/.local/share/kgc-tools/lib DISPLAY=:99 ./build/linux-release/src/main/KGroundControl
```

The library path above supplies libxcb-cursor on the development server.
Use `QT_QPA_PLATFORM=offscreen` for a display-free smoke run. `--data-dir PATH`
overrides the data directory; `--log-level debug` enables debug events.
Use the `linux-debug` presets for development and `KGC_EXTRA_TESTS_DIR` for external tests.

## Windows

From an MSVC 2022 developer shell, set `QT_ROOT` to Qt 6.12.0 msvc2022_64,
then configure, build and test with the `windows-release` presets.
Download the CI zip and extract all files before launching `KGroundControl.exe`.
For an unsigned build, SmartScreen may require **More info → Run anyway**.

CI produces an AppImage and a Windows zip containing LICENSE and third-party notices.
AppImage systems without FUSE can use `--appimage-extract-and-run`.
