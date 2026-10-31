# [0.1.0 T8] specs/features/0.1.0-link.md
import re
from checks._cpp import sources, line, clean


def check(ctx):
    """펌웨어에 따른 판정을 플러그인 안에 모으도록 확인해요.

    입력: 검사 문맥.
    출력: 위반 위치.
    실패: 없음.
    """
    allowed = ('src/platform/vehicle/firmware/', 'src/features/fcFlash/',
               'src/features/logs/', 'src/features/params/')
    names = {'px4', 'ardupilot', 'apm'}
    for path in ctx['files']:
        if path.startswith('src/features/') and names.intersection(path.lower().split('/')[3:-1]):
            yield path, 1, '기능 모듈의 펌웨어별 폴더 금지'
    for path, text in sources(ctx, qml=True):
        if path.startswith(allowed):
            continue
        pattern = r'\bMAV_AUTOPILOT_(?:PX4|ARDUPILOTMEGA)\b'
        for match in re.finditer(pattern, clean(text, True)):
            yield path, line(text, match.start()), '플러그인 밖 펌웨어 분기 금지'
        literal = r'(?:u8|[uUL])?["\'](?:PX4|ArduPilot|APM)["\'](?:_s)?'
        value = r'(?:\b(?:QStringLiteral|QLatin1String|QString)\s*\(\s*' + literal + r'\s*\)|' + literal + r')'
        pattern = (r'(?:===?|!==?)\s*' + value + r'|' + value + r'\s*(?:===?|!==?)'
                   + r'|\b(?:compare|contains|startsWith|endsWith)\s*\(\s*' + value)
        for match in re.finditer(pattern, text):
            yield path, line(text, match.start()), '플러그인 밖 펌웨어 문자열 비교 금지'
