# [0.1.0 T8] specs/features/0.1.0-link.md
import re
from checks._cpp import sources, calls, TOKEN, line


def check(ctx):
    """한국어 화면 문구가 번역 목록에서 빠지지 않게 확인해요.

    입력: 검사 문맥.
    출력: 위반 위치.
    실패: 없음.
    """
    for path, text in sources(ctx):
        translated = list(calls(text, r'tr|QT_TR_NOOP|QT_TRANSLATE_NOOP'))
        for match in TOKEN.finditer(text):
            if match[0].startswith(('"', 'R"')) and re.search('[가-힣ᄀ-ᇿ㄰-㆏]', match[0]):
                if not any(start <= match.start() < end for start, end in translated):
                    yield path, line(text, match.start()), '한국어 문자열에 번역 표시 필요'
