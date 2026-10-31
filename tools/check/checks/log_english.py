# [BASE 10] specs/BASE.md
import re
from checks._cpp import sources, calls, TOKEN, line


def check(ctx):
    """로그 문구가 화면 번역과 섞이지 않도록 한글 리터럴을 찾아요.

    입력: 파일 경로와 원문을 담은 검사 문맥이에요.
    출력: 로그 호출 안 한글 리터럴의 경로·줄 번호·위반 문구예요.
    실패: 위반은 결과로 돌려주고, 주석과 로그 밖 문자열은 보고하지 않아요.
    """
    for path, text in sources(ctx):
        for start, end in calls(text, r'KGC_LOG_(?:DEBUG|INFO|WARN|ERROR)'):
            for literal in TOKEN.finditer(text, start, end):
                if literal[0].startswith(('"', 'R"')) and re.search('[가-힣ᄀ-ᇿ㄰-㆏]', literal[0]):
                    yield path, line(text, literal.start()), '로그 메시지에 한글 금지'
