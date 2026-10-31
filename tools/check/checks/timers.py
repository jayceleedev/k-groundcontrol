# [0.1.0 T8] specs/features/0.1.0-link.md
import re
from checks._cpp import sources, clean, line


def check(ctx):
    """반복 작업과 화면 타이머를 정해진 주인에게 모아요.

    입력: 검사 문맥.
    출력: 위반 위치.
    실패: 없음.
    """
    for path, text in sources(ctx, True):
        if path.startswith('src/platform/app/scheduler/') or path == 'src/platform/ui/components/TimerBox.qml':
            continue
        pattern = r'\b(?:QTimer|QBasicTimer)\b|\b(?:singleShot|startTimer)\s*\(|\bTimer\s*\{'
        for match in re.finditer(pattern, clean(text, True)):
            yield path, line(text, match.start()), '공용 스케줄러 밖 타이머 금지'
