# [0.1.0 T8] specs/features/0.1.0-link.md
import re
from checks._cpp import sources, TOKEN, line


def check(ctx):
    """운영 설정을 문자열 키 대신 생성된 접근자로 읽도록 확인해요.

    입력: 검사 문맥.
    출력: 위반 위치.
    실패: 없음.
    """
    owners = set()
    for path, text in ctx['files'].items():
        if path.endswith('CMakeLists.txt'):
            owners.update(re.findall(r'kgc_use_settings\s*\(\s*\S+\s+([\w]+)', text))
    for path, text in sources(ctx, qml=True):
        for match in TOKEN.finditer(text):
            if not match[0].startswith(('"', "'")):
                continue
            value = match[0][1:-1]
            if re.fullmatch(r'\w+\.\w+', value) and value.split('.')[0] in owners:
                yield path, line(text, match.start()), '운영 설정 키 문자열 대신 생성 접근자 사용'
