# [0.0.1 T11] specs/features/0.0.1-shell.md
import re
from pathlib import Path


def check(ctx):
    """승인된 구조 규칙의 위반 위치를 돌려줘요."""
    for path, text in ctx["files"].items():
        if not path.startswith("src/features/") or not any(s in path for s in ("/api/", "/internal/")):
            continue
        root = ctx["owner"](path)
        for number, line in enumerate(text.splitlines(), 1):
            match = re.search(r'#\s*include\s*[<"]([^">]+)', line)
            if match and ("ui/" in match[1] or any(
                    p.startswith(root + "/ui/") and p.endswith("/" + match[1]) for p in ctx["files"])):
                yield path, number, "화면 방향 헤더 포함 금지"
