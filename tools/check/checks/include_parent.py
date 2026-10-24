# [0.0.1 T11] specs/features/0.0.1-shell.md
import re
from pathlib import Path


def check(ctx):
    """승인된 구조 규칙의 위반 위치를 돌려줘요."""
    for path, text in ctx["files"].items():
        for number, line in enumerate(text.splitlines(), 1):
            if re.search(r'^\s*#\s*include\s*[<"][^">]*\.\.', line):
                yield path, number, "상위 폴더 상대 경로 포함 금지"
