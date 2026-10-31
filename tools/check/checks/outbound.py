# [0.1.0 T8] specs/features/0.1.0-link.md
import re
from checks._cpp import sources, clean, line


def check(ctx):
    """기체 송신과 명령 패킹의 주인을 확인해요.

    입력: 검사 문맥.
    출력: 위반 위치.
    실패: 없음.
    """
    for path, text in sources(ctx):
        code = clean(text, True)
        if not path.startswith('src/platform/vehicle/command/'):
            for match in re.finditer(r'\bmavlink_msg_command_(?:long|int)_(?:pack|encode)(?:_chan|_status)?\s*\(', code):
                yield path, line(text, match.start()), '명령 큐 밖 명령 패킹 금지'
        if path.startswith(('src/platform/comm/', 'src/platform/vehicle/outbound/')):
            continue
        pattern = r'\b(?:writeBytes|sendBytes|writeMessage|sendMessage)\s*\(|\b(?:link|worker)\w*\s*(?:->|\.)\s*write\s*\('
        for match in re.finditer(pattern, code):
            yield path, line(text, match.start()), '송신 문 밖 링크 쓰기 금지'
