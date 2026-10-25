# [0.0.1 T11] specs/features/0.0.1-shell.md
import re
from pathlib import Path


def check(ctx):
    """승인된 구조 규칙의 위반 위치를 돌려줘요."""
    for path, text in ctx["files"].items():
        if path.endswith(".qml"):
            for number, line in enumerate(text.splitlines(), 1):
                # ToolTip.text만 전용 규칙이 맡고, 다른 객체의 본문 속성도 여기서 확인해요.
                matches = re.finditer(r'(?<!\w)(?:(\w+)\.)?text\s*:\s*["\']', line)
                if any(match[1] != "ToolTip" for match in matches):
                    yield path, number, "번역하지 않은 화면 문자열"
