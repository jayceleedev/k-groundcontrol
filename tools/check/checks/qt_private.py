# [0.5.0 T12] specs/features/0.5.0-map.md
import re
from checks._cpp import sources, line


def check(ctx):
    """Qt 비공개 API(비공개 헤더와 *Private CMake 대상)를 지도 geoservice 한 곳에만 둬요.

    입력: 검사 문맥.
    출력: 위반 위치.
    실패: 없음.
    """
    for path, text in sources(ctx):
        if path.startswith('src/features/map/internal/geoservice/'):
            continue
        for match in re.finditer(r'#\s*include\s*[<"][^">]*_p\.h[">]', text):
            yield path, line(text, match.start()), 'Qt 비공개 헤더는 지도 geoservice 밖 금지'
    for path, text in ctx['files'].items():
        if not path.endswith('CMakeLists.txt') or path == 'src/features/map/CMakeLists.txt':
            continue
        code = re.sub(r'#[^\n]*', lambda m: ' ' * len(m[0]), text)
        for match in re.finditer(r'\b(?:Qt6::)?[A-Z]\w*Private\b', code):
            yield path, line(text, match.start()), 'Qt 비공개 대상은 지도 밖 금지'
