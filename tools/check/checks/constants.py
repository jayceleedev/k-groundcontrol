# [0.1.0 T8] specs/features/0.1.0-link.md
import re
from checks._cpp import sources, clean, line, calls

NUMBER = re.compile(r"(?<![\w.])(?P<number>0[xX][\da-fA-F']+|\d[\d']*(?:\.\d+)?(?:[eE][+-]?\d+)?)(?P<suffix>ms|us|ns|s|min|h|[uUlLfF]+)?\b")


def timer_arguments(code):
    """중첩된 기간 생성식도 타이머 인자로 알아봐요.

    입력: 주석과 문자열을 가린 코드.
    출력: 기간 인자의 시작과 끝 위치.
    실패: 닫히지 않은 호출은 파일 끝까지 살펴봐요.
    """
    pattern = r'setInterval|startTimer|singleShot|registerTask|Timeout(?:\s+\w+)?'
    for start, end in calls(code, pattern):
        opening = code.index('(', start, end)
        name = code[start:opening].strip()
        argument, depth, begin = 0, 0, opening + 1
        for pos in range(begin, end):
            char = code[pos]
            if (char == ',' and depth == 0) or pos == end - 1:
                if name.startswith('Timeout') or argument == (1 if name == 'registerTask' else 0):
                    yield begin, pos
                argument, begin = argument + 1, pos + 1
            depth += (char in '([{') - (char in ')]}')


def check(ctx):
    """고정값이 여러 곳에 흩어지지 않도록 정해진 수의 위치를 확인해요.

    입력: 검사 문맥.
    출력: 위반 위치.
    실패: 없음.
    """
    for path, text in sources(ctx):
        if path.endswith(('/Constants.h', '/ModuleId.h')) or path.startswith('src/platform/app/constants/'):
            continue
        code = clean(text, True)
        timers = list(timer_arguments(code))
        for match in NUMBER.finditer(code):
            digits = match['number'].replace("'", '')
            chrono = match['suffix'] in {'ms', 'us', 'ns', 'min', 'h', 's'}
            hexadecimal = digits.lower().startswith('0x')
            value = int(digits, 16) if hexadecimal else float(digits)
            timer = any(start <= match.start() < end for start, end in timers)
            checked = chrono or timer or (hexadecimal and len(digits) >= 6)
            checked |= not hexadecimal and value >= 100 and value not in {360, 180, 90, 1000, 60}
            before = code[max(0, match.start() - 80):match.start()]
            after = code[match.end():match.end() + 8]
            offset = not (chrono or timer) and value < 256 and (
                re.search(r'(?:\[|<<|>>|\+|-)\s*$', before) or re.match(r'\s*\]', after))
            if checked and not offset:
                yield path, line(text, match.start()), '고정값은 Constants.h 또는 공용 상수에 선언'
