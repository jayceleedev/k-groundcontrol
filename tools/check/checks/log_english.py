# [BASE 10] specs/BASE.md
import re

CALL = re.compile(r"\bKGC_LOG_(?:DEBUG|INFO|WARN|ERROR)\s*\(")
LITERAL = re.compile(r'"(?:[^"\\\n]|\\.)*"')
HANGUL = re.compile(r"[가-힣ᄀ-ᇿ㄰-㆏]")


def call_end(text, start):
    """여는 괄호 바로 뒤에서 시작해, 문자열 안의 괄호는 건너뛰고 짝이 맞는 닫는 괄호 위치를 찾아요."""
    depth, index = 1, start
    while index < len(text) and depth:
        if text[index] == '"':
            match = LITERAL.match(text, index)
            index = match.end() if match else index + 1
            continue
        depth += {"(": 1, ")": -1}.get(text[index], 0)
        index += 1
    return index


def check(ctx):
    """로그 매크로 호출 안의 문자열 리터럴에 한글이 있으면 그 줄을 돌려줘요(로그는 영어로만 남겨요).

    입력: ctx(run.py가 만든 검사 문맥, files는 경로 → 내용)
    출력: (경로, 줄 번호, 메시지)를 하나씩 내요
    실패: 없어요(위반은 출력으로만 알려요)
    """
    for path, text in ctx["files"].items():
        if not path.startswith("src/") or not path.endswith((".cpp", ".h")):
            continue
        if path.startswith("src/platform/app/log/"):
            continue
        for call in CALL.finditer(text):
            end = call_end(text, call.end())
            for literal in LITERAL.finditer(text, call.end(), end):
                if HANGUL.search(literal.group()):
                    yield path, text.count("\n", 0, literal.start()) + 1, "로그 메시지에 한글 금지"
