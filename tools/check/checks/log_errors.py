# [0.1.0 T8] specs/features/0.1.0-link.md
import re
from checks._cpp import sources, clean, line

ENUM = re.compile(r'enum\s+class\s+(\w+(?:Event|Error))\s*(?::[^{}]+)?\{([^}]+)\}', re.S)


def entries(body):
    """생략된 열거 번호도 계산해 중복을 놓치지 않아요.

    입력: 열거 본문.
    출력: 이름과 번호.
    실패: 계산할 수 없는 식은 번호 대신 식을 유지해요.
    """
    value = -1
    for item in body.split(','):
        match = re.fullmatch(r'\s*(\w+)\s*(?:=\s*(.*?))?\s*', item)
        if not match:
            continue
        if match[2]:
            try:
                value = int(match[2], 0)
            except ValueError:
                value = match[2]
        else:
            value = value + 1 if isinstance(value, int) else str(value) + '+1'
        yield match[1], value


def registrations(ctx):
    """모듈별 사건과 오류의 등록 이름을 모아요.

    입력: 검사 문맥.
    출력: 등록 이름 집합과 위반 목록.
    실패: 없음.
    """
    registered, issues, numbers, module_ids = set(), [], {}, {}
    for path, text in sources(ctx):
        owner = ctx['owner'](path)
        for match in re.finditer(r'\bkModuleId\s*(?:=\s*)?\{\s*(\d+)\s*\}', clean(text, True)):
            value = int(match[1])
            if not path.endswith('/ModuleId.h') or value in module_ids:
                issues.append((path, line(text, match.start()), '모듈 번호 등록 위치 또는 중복 오류'))
            module_ids[value] = path
        for match in ENUM.finditer(clean(text, True)):
            kind = 'Event' if match[1].endswith('Event') else 'Error'
            expected = '/internal/Events.h' if kind == 'Event' else '/Errors.h'
            if not path.endswith(expected) or (kind == 'Error' and '/api/' not in path):
                issues.append((path, line(text, match.start()), '사건·오류 열거의 등록 위치 오류'))
            for name, value in entries(match[2]):
                key = owner, kind, value
                if key in numbers:
                    issues.append((path, line(text, match.start()), '모듈 안 사건·오류 번호 중복'))
                numbers[key] = name
                registered.add((match[1], name))
    return registered, issues


def check(ctx):
    """로그 호출과 사건·오류 번호가 모듈 등록부를 따르는지 확인해요.

    입력: 검사 문맥.
    출력: 위반 위치.
    실패: 없음.
    """
    registered, issues = registrations(ctx)
    yield from issues
    for path, text in sources(ctx):
        code = clean(text, True)
        for match in re.finditer(r'\b(?:qDebug|qInfo|qWarning|qCritical|qCDebug|qCInfo|qCWarning|qCCritical)\s*\(', code):
            yield path, line(text, match.start()), '수준 로그 매크로 밖 직접 로그 호출 금지'
        for match in re.finditer(r'\b(\w+(?:Event|Error))::(\w+)\b', code):
            # Qt의 사건 번호는 앱 모듈 등록부가 소유하지 않아요.
            if match[1] == 'QEvent':
                continue
            if (match[1], match[2]) not in registered:
                yield path, line(text, match.start()), '등록되지 않은 사건·오류 이름'
        for match in re.finditer(r'\bKGC_LOG_(?:DEBUG|INFO|WARN|ERROR)\s*\(\s*[^,]+,\s*([^,]+),', code):
            if code[max(0, match.start() - 8):match.start()].strip().endswith('define'):
                continue
            if not re.fullmatch(r'\s*(?:\w+::)*\w+Event::\w+\s*', match[1]):
                yield path, line(text, match.start()), '로그 사건에 등록 열거 필요'
