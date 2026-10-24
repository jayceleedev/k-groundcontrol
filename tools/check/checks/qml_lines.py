# [0.0.1 T11] specs/features/0.0.1-shell.md
import re
from pathlib import Path


def check(ctx):
    """승인된 구조 규칙의 위반 위치를 돌려줘요."""
    for path, text in ctx["files"].items():
        if path.endswith(".qml") and len(text.splitlines()) > 300:
            yield path, 301, "파일 물리 줄 수 초과: 300"
