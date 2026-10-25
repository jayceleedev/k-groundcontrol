# [0.0.2 T6] specs/features/0.0.2-theme.md
"""모든 테마가 같은 토큰 계약을 채우는지 확인해요."""
import re


def check(ctx):
    """필수 토큰과 실제로 값을 준 토큰의 차이를 알려줘요."""
    folder = 'src/platform/ui/themes/'
    base = folder + 'ThemeBase.qml'
    if base not in ctx['files']:
        return
    required = set(re.findall(r'required\s+property\s+\w+\s+(\w+)', ctx['files'][base]))
    for path, text in ctx['files'].items():
        if not path.startswith(folder) or not path.endswith('.qml') or path == base:
            continue
        assigned = set(re.findall(r'^\s*(\w+)\s*:', text, re.M))
        for token in sorted(required - assigned):
            yield path, 1, '필수 토큰 누락: ' + token
        for token in sorted(assigned - required):
            yield path, 1, '등록되지 않은 토큰: ' + token
