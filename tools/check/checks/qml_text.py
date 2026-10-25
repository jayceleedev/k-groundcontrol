# [0.0.1 T11] specs/features/0.0.1-shell.md
import re
from pathlib import Path


def check(ctx):
    """승인된 구조 규칙의 위반 위치를 돌려줘요."""
    for path, text in ctx["files"].items():
        if path.endswith(".qml"):
            for number, line in enumerate(text.splitlines(), 1):
                # 붙임 속성은 전용 규칙이 맡으므로 일반 본문 속성만 확인해요.
                if re.search(r'(?<![\w.])text\s*:\s*["\']', line):
                    yield path, number, "번역하지 않은 화면 문자열"
