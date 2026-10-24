# [0.0.1 T11] specs/features/0.0.1-shell.md
import re
from pathlib import Path


def check(ctx):
    """승인된 구조 규칙의 위반 위치를 돌려줘요."""
    import lizard
    for path, text in ctx["files"].items():
        if Path(path).suffix not in {".cpp", ".h", ".c", ".py"}:
            continue
        for function in lizard.analyze_file.analyze_source_code(path, text).function_list:
            if function.end_line - function.start_line + 1 > 60:
                yield path, function.start_line, "함수 물리 줄 수 초과: " + function.name
