#!/usr/bin/env python3
# [0.0.1 T11] specs/features/0.0.1-shell.md
"""작업 트리와 커밋의 모듈 경계를 같은 규칙으로 확인해요."""
import argparse
import importlib.util
import re
import subprocess
import sys
from pathlib import Path

sys.dont_write_bytecode = True
ROOT = Path(__file__).resolve().parents[2]
RANKS = {name: rank for rank, names in enumerate([
    "shell", "notify ai appSettings reference analyze link uiGallery secondPanel", "fly mission vehicleInfo preflight logs",
    "layout", "sensors radio flightModes power failsafe motors tuning fcFlash esc elrs tracker",
    "map", "video terrain gcsposition", "params", "extras"], 1) for name in names.split()}


def git(*args):
    return subprocess.check_output(["git", "-C", str(ROOT), *args]).decode("utf-8")


def source_file(path):
    return path.startswith(("src/", "tests/", "tools/", "cmake/", "tracker/")) and (
        Path(path).suffix in {".h", ".cpp", ".c", ".qml", ".py", ".cmake"}
        or path.endswith("CMakeLists.txt"))


def snapshot(revision=None):
    if revision:
        paths = git("ls-tree", "-r", "--name-only", revision).splitlines()
        return {p: git("show", f"{revision}:{p}") for p in paths
                if source_file(p) or p.endswith("MODULE.md")}
    paths = git("ls-files", "--cached", "--others", "--exclude-standard").splitlines()
    return {p: (ROOT / p).read_text() for p in set(paths)
            if (source_file(p) or p.endswith("MODULE.md")) and (ROOT / p).is_file()}


def modules(files):
    found = set()
    for path in files:
        match = re.match(r"(src/(?:features/[^/]+|platform/ui|platform/(?:app|comm|vehicle)/[^/]+))/", path)
        if match:
            found.add(match[1])
    return sorted(found, key=len, reverse=True)


def owner(path, roots):
    return next((root for root in roots if path.startswith(root + "/")), None)


def layer(module):
    if not module:
        return None
    return {"app": 0, "comm": 1, "vehicle": 2, "ui": 2.5}.get(module.split("/")[2])


def target(module):
    return "kgc_" + module.removeprefix("src/platform/").removeprefix("src/features/").replace("/", "_")


def links(content):
    clean = re.sub(r"#[^\n]*", "", content)
    return [re.findall(r"[\w:]+", m) for m in re.findall(
        r"target_link_libraries\s*\(([^)]*)\)", clean, re.I | re.S)]


def context(files, changed=(), previous=None):
    roots = modules(files)
    targets = {target(root): root for root in roots}
    graph = {root: set() for root in roots}
    for root in roots:
        for words in links(files.get(root + "/CMakeLists.txt", "")):
            for dependency in words[1:]:
                if dependency in targets:
                    graph[root].add(targets[dependency])
    return {"files": files, "roots": roots, "owner": lambda p: owner(p, roots),
            "graph": graph, "ranks": RANKS, "layer": layer, "links": links,
            "changed": set(changed), "previous": previous or {}, "source_file": source_file}


def load_checks():
    checks = []
    for path in sorted((ROOT / "tools/check/checks").glob("*.py")):
        spec = importlib.util.spec_from_file_location(path.stem, path)
        module = importlib.util.module_from_spec(spec)
        spec.loader.exec_module(module)
        checks.append((path.stem, module.check))
    return checks


def inspect(ctx, checks):
    failed = False
    for rule, check in checks:
        for path, line, message in check(ctx):
            print(f"{rule}: {path}:{line} {message}")
            failed = True
    return failed


def check_range(value, checks):
    before, after = value.split("..", 1)
    revision = after if not before.strip("0") else value
    failed = False
    for commit in git("rev-list", "--reverse", revision).splitlines():
        parent = git("rev-list", "--parents", "-n", "1", commit).split()[1:]
        previous = snapshot(parent[0]) if parent else {}
        files = snapshot(commit)
        changed = {p for p in files.keys() | previous.keys() if files.get(p) != previous.get(p)}
        failed |= inspect(context(files, changed, previous), checks)
    return failed


def main():
    try:
        import lizard
    except ImportError:
        print("lizard: tools/check/requirements.txt:1 검사 도구 누락, 설치: pip install -r tools/check/requirements.txt")
        return 1
    parser = argparse.ArgumentParser()
    parser.add_argument("--range")
    args = parser.parse_args()
    checks = load_checks()
    if args.range:
        return int(check_range(args.range, checks))
    failed = inspect(context(snapshot()), checks)
    changed = git("diff", "--cached", "--name-only").splitlines()
    if changed:
        tree = git("write-tree").strip()
        failed |= inspect(context(snapshot(tree), changed, snapshot("HEAD")), checks)
    return int(failed)


if __name__ == "__main__":
    sys.exit(main())
