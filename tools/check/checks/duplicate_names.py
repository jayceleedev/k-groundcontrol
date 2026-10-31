# [0.1.0 T8] specs/features/0.1.0-link.md
import re
from checks._cpp import sources, clean


def class_ranges(code):
    """클래스 안 함수는 자유 함수 중복 검사에서 구분해요.

    입력: 문자열과 주석을 가린 코드.
    출력: 클래스 이름과 본문의 위치.
    실패: 미완성 선언은 건너뛰어요.
    """
    for match in re.finditer(r'\b(?:class|struct)\s+(\w+)[^;{}]*\{', code):
        end, depth = match.end(), 1
        while end < len(code) and depth:
            depth += (code[end] == '{') - (code[end] == '}')
            end += 1
        yield match[1], match.start(), end


def check(ctx):
    """같은 역할의 자유 함수가 여러 모듈에 복제되는지 확인해요.

    입력: 검사 문맥.
    출력: 위반 위치.
    실패: 없음.
    """
    import lizard
    seen = {}
    for path, text in sources(ctx):
        owner = ctx['owner'](path)
        if not owner:
            continue
        code = clean(text, True)
        classes = list(class_ranges(code))
        class_names = {name for name, _, _ in classes}
        for function in lizard.analyze_file.analyze_source_code(path, text).function_list:
            name = function.name.split('::')[-1]
            if name in {'registerUi', 'make', 'errorText'} or 'operator' in name:
                continue
            if any(part in class_names for part in function.name.split('::')[:-1]):
                continue
            position = sum(len(row) for row in code.splitlines(keepends=True)[:function.start_line - 1])
            if any(start <= position < end for _, start, end in classes):
                continue
            # 헤더에 선언된 클래스의 바깥 정의도 자유 함수로 보지 않아요.
            if '::' in function.name and function.name.split('::')[-2][:1].isupper():
                continue
            if name in seen and seen[name] != owner:
                yield path, function.start_line, '모듈 사이 자유 함수 이름 중복: ' + name
            seen[name] = owner
