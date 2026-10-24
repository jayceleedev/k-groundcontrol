# [0.0.1 T11] specs/features/0.0.1-shell.md
import re
from pathlib import Path


def check(ctx):
    """승인된 구조 규칙의 위반 위치를 돌려줘요."""
    for path, text in ctx["files"].items():
        if path.startswith("src/platform/"):
            for number, line in enumerate(text.splitlines(), 1):
                if re.search(r'#\s*include\s*[<"]kgc/(' + "|".join(ctx["ranks"]) + r')/', line):
                    yield path, number, "바탕에서 기능 헤더 포함 금지"
    for root, deps in ctx["graph"].items():
        if root.startswith("src/platform/") and any(d.startswith("src/features/") for d in deps):
            yield root + "/CMakeLists.txt", 1, "바탕에서 기능 연결 금지"
