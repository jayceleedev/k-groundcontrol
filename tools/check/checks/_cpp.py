# [0.1.0 T8] specs/features/0.1.0-link.md
"""문자열과 주석을 구분해 검사 위치가 실제 코드와 맞도록 도와줘요."""
import re

TOKEN = re.compile(r'R"(?P<delimiter>[^ ()\\\t\r\n]{0,16})\(.*?\)(?P=delimiter)"|"(?:\\.|[^"\\])*"|(?P<character>(?<![\w])(?:u8|[uUL])?\'(?:\\[^\r\n]|[^\'\\\r\n])*\')|//[^\n]*|/\*.*?\*/', re.S)


def clean(text, strings=False, qml=False):
    """줄 번호를 유지하면서 주석과 필요하면 문자열을 가려요.

    입력: 소스, 문자열을 가릴지 여부, 작은따옴표도 문자열인 QML 여부.
    출력: 같은 길이의 소스.
    실패: 없음.
    """
    return TOKEN.sub(lambda m: re.sub(r'[^\n]', ' ', m[0])
                     if strings or m[0].startswith('/') or (m.group('character') and not qml) else m[0], text)


def sources(ctx, qml=False):
    """검사 대상 원본 코드만 골라요.

    입력: 검사 문맥과 화면 코드 포함 여부.
    출력: 경로와 주석을 가린 코드.
    실패: 없음.
    """
    for path, text in ctx['files'].items():
        if path.startswith('src/') and path.endswith(('.cpp', '.h', '.c') + (('.qml',) if qml else ())):
            yield path, clean(text, qml=path.endswith('.qml'))


def line(text, position):
    """오류를 편집기에서 찾을 수 있도록 줄 번호를 구해요.

    입력: 소스와 문자 위치.
    출력: 첫 줄을 1로 센 번호.
    실패: 없음.
    """
    return text.count('\n', 0, position) + 1


def calls(text, pattern):
    """줄을 나눈 호출에서도 괄호 안을 정확하게 검사해요.

    입력: 소스와 호출 이름 정규식.
    출력: 호출 시작과 끝 위치.
    실패: 닫히지 않은 호출은 파일 끝까지 읽어요.
    """
    masked = clean(text, True)
    for match in re.finditer(r'\b(?:' + pattern + r')\s*\(', masked):
        depth, end = 1, match.end()
        while end < len(masked) and depth:
            depth += (masked[end] == '(') - (masked[end] == ')')
            end += 1
        yield match.start(), end


def check(ctx):
    """공용 도우미는 단독으로 위반을 내지 않아요.

    입력: 검사 문맥.
    출력: 빈 결과.
    실패: 없음.
    """
    return ()
