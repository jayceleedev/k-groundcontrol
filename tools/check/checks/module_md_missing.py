# [0.0.1 T11] specs/features/0.0.1-shell.md
import re
from pathlib import Path


def check(ctx):
    """승인된 구조 규칙의 위반 위치를 돌려줘요."""
    roots = set(ctx["roots"])
    roots.update(p.rsplit("/", 1)[0] for p in ctx["files"] if p in {
        "src/platform/app/MODULE.md", "src/platform/comm/MODULE.md", "src/platform/vehicle/MODULE.md"})
    if any(p.startswith("src/platform/") for p in ctx["files"]):
        roots.update({"src/platform/app", "src/platform/comm", "src/platform/vehicle"})
    for root in sorted(roots):
        if root + "/MODULE.md" not in ctx["files"]:
            yield root + "/MODULE.md", 1, "모듈 설명서 누락"
