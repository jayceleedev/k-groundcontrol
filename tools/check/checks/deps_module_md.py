# [0.0.1 T11] specs/features/0.0.1-shell.md
import re
from pathlib import Path


def check(ctx):
    """승인된 구조 규칙의 위반 위치를 돌려줘요."""
    for root in ctx["roots"]:
        path = root + "/CMakeLists.txt"
        if path in ctx["changed"] and ctx["links"](ctx["files"].get(path, "")) != ctx["links"](ctx["previous"].get(path, "")):
            if root + "/MODULE.md" not in ctx["changed"]:
                yield root + "/MODULE.md", 1, "모듈 의존 변경에 설명서 미동반"
