# [0.0.1 T11] specs/features/0.0.1-shell.md
import re
from pathlib import Path


def check(ctx):
    """승인된 구조 규칙의 위반 위치를 돌려줘요."""
    for path, text in ctx["files"].items():
        if not path.endswith(".qml"):
            continue
        root = ctx["owner"](path)
        current = root.split("/")[-1] if root else ""
        for number, line in enumerate(text.splitlines(), 1):
            match = re.match(r'\s*import\s+([\w.]+)', line)
            if not match:
                continue
            name = match[1]
            allowed = name.startswith(("QtQuick", "QtQml")) or name == "Kgc.Ui"
            if root and root.startswith("src/features/"):
                allowed |= name in {"QtLocation", "QtPositioning", "QtMultimedia", "QtGraphs", "QtQuick3D"}
                feature = next((f for f in ctx["ranks"] if "Kgc." + f[0].upper() + f[1:] == name), None)
                allowed |= feature == current or feature is not None and ctx["ranks"][feature] > ctx["ranks"].get(current, 9)
            if not allowed:
                yield path, number, "허용하지 않는 화면 모듈 가져오기: " + name
