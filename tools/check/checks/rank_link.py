# [0.0.1 T11] specs/features/0.0.1-shell.md
import re
from pathlib import Path


def check(ctx):
    """승인된 구조 규칙의 위반 위치를 돌려줘요."""
    for root, deps in ctx["graph"].items():
        if not root.startswith("src/features/"):
            continue
        rank = ctx["ranks"].get(root.split("/")[-1], 9)
        for dep in deps:
            if dep.startswith("src/features/"):
                other = ctx["ranks"].get(dep.split("/")[-1], 9)
                if other < rank:
                    yield root + "/CMakeLists.txt", 1, "기능 순위 연결 위반: " + dep
