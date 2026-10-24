# [0.0.1 T11] specs/features/0.0.1-shell.md
import re
from pathlib import Path


def check(ctx):
    """승인된 구조 규칙의 위반 위치를 돌려줘요."""
    for path, text in ctx["files"].items():
        if path.endswith(".qml"):
            for number, line in enumerate(text.splitlines(), 1):
                if re.search(r'\bconsole\s*\.\s*log\s*\(', line):
                    yield path, number, "화면 콘솔 로그 직접 호출 금지"
