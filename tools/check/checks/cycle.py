# [0.0.1 T11] specs/features/0.0.1-shell.md
import re
from pathlib import Path


def check(ctx):
    """승인된 구조 규칙의 위반 위치를 돌려줘요."""
    def reaches(start, node, seen):
        for child in ctx["graph"][node]:
            if child == start or child not in seen and reaches(start, child, seen | {child}):
                return True
        return False
    for root in ctx["graph"]:
        if reaches(root, root, {root}):
            yield root + "/CMakeLists.txt", 1, "모듈 의존 순환"
