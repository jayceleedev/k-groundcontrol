# [0.1.0 T8] specs/features/0.1.0-link.md
import re
from checks._cpp import sources, clean, line


def check(ctx):
    """시리얼 입출력과 환경 설정을 소유 모듈 안에 유지해요.

    입력: 검사 문맥.
    출력: 위반 위치.
    실패: 없음.
    """
    for path, text in sources(ctx):
        code = clean(text, True)
        if not path.startswith('src/platform/app/boot/'):
            pattern = r'\b(?:qEnvironmentVariable\w*|qgetenv|getenv|secure_getenv|_wgetenv|_dupenv_s)\s*\('
            pattern += r'|\bQProcessEnvironment\b'
            for match in re.finditer(pattern, code):
                yield path, line(text, match.start()), '부팅 설정 밖 환경 변수 읽기 금지'
        if not path.startswith(('src/platform/comm/links/serial/', 'src/platform/comm/rawport/')):
            for match in re.finditer(r'\bQSerialPort\b', code):
                yield path, line(text, match.start()), '시리얼 소유 모듈 밖 포트 사용 금지'
