# [0.0.1 T11] specs/features/0.0.1-shell.md
import re
from pathlib import Path


def check(ctx):
    """승인된 구조 규칙의 위반 위치를 돌려줘요."""
    for path, text in ctx["files"].items():
        current = ctx["layer"](ctx["owner"](path))
        if current is None:
            continue
        for number, line in enumerate(text.splitlines(), 1):
            match = re.search(r'#\s*include\s*[<"]kgc/(app|comm|vehicle|ui)/', line)
            if match and {"app": 0, "comm": 1, "vehicle": 2, "ui": 2.5}[match[1]] > current:
                yield path, number, "상위 바탕 층 헤더 포함 금지"
    for root, deps in ctx["graph"].items():
        current = ctx["layer"](root)
        if current is not None and any(ctx["layer"](d) is not None and ctx["layer"](d) > current for d in deps):
            yield root + "/CMakeLists.txt", 1, "상위 바탕 층 연결 금지"
