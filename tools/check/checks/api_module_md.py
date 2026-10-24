# [0.0.1 T11] specs/features/0.0.1-shell.md
import re
from pathlib import Path


def check(ctx):
    """승인된 구조 규칙의 위반 위치를 돌려줘요."""
    for root in ctx["roots"]:
        if any(p.startswith(root + "/api/") for p in ctx["changed"]) and root + "/MODULE.md" not in ctx["changed"]:
            yield root + "/MODULE.md", 1, "공개 인터페이스 변경에 설명서 미동반"
