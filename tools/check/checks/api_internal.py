# [0.0.1 T11] specs/features/0.0.1-shell.md
import re
from pathlib import Path


def check(ctx):
    """승인된 구조 규칙의 위반 위치를 돌려줘요."""
    for path, text in ctx["files"].items():
        if "/api/" not in path:
            continue
        root = ctx["owner"](path)
        for number, line in enumerate(text.splitlines(), 1):
            match = re.search(r'^\s*#\s*include\s*[<"]([^">]+)', line)
            if match and ("internal/" in match[1] or any(
                    p.startswith(root + "/internal/") and p.endswith("/" + match[1]) for p in ctx["files"])):
                yield path, number, "공개 헤더에서 내부 헤더 포함 금지"
