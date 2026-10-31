# [0.1.0 T8] specs/features/0.1.0-link.md
import re
from checks._cpp import sources, line


def check(ctx):
    """방언 설정을 우회하거나 화면에 통신 형식을 노출하는 헤더를 찾아요.

    입력: 검사 문맥.
    출력: 위반 위치.
    실패: 없음.
    """
    wrapper = 'src/platform/comm/mavlink/api/kgc/comm/mavlink/Mavlink.h'
    public = {'Vehicle.h', 'VehicleRegistry.h', 'VehicleStates.h',
              'VehicleStateOwner.h', 'LinkService.h'}
    for path, text in sources(ctx):
        forbidden = (path.startswith(('src/platform/app/', 'src/platform/ui/'))
                     or '/ui/' in path or path.rsplit('/', 1)[-1] in public)
        for match in re.finditer(r'#\s*include\s*([<"])([^">]+)[">]', text):
            header = match[2]
            if header == 'kgc/app/constants/mavlink.h':
                continue
            name = header.rsplit('/', 1)[-1]
            protocol = ('mavlink' in header.lower()
                        or re.search(r'(?:^|/)(?:ardupilotmega|common)/.*\.h$', header)
                        or re.search(r'(?:^|/)(\w+)/\1\.h$', header))
            exposed = protocol or name in {'LinkManager.h', 'VehicleSender.h'}
            raw = name.lower() == 'mavlink.h' or name.lower().startswith('mavlink_')
            direct = protocol and (not header.startswith('kgc/')
                                   or (raw and (header != 'kgc/comm/mavlink/Mavlink.h' or match[1] != '<')))
            if (forbidden and exposed) or (direct and path != wrapper):
                yield path, line(text, match.start()), 'MAVLink 헤더 경계 위반'
