# [0.0.2 T13] specs/features/0.0.2-theme.md
"""화면의 색과 고정 크기가 테마에 모이도록 확인해요."""
import re

COLORS = set(('aliceblue antiquewhite aqua aquamarine azure beige bisque black '
    'blanchedalmond blue blueviolet brown burlywood cadetblue chartreuse chocolate '
    'coral cornflowerblue cornsilk crimson cyan darkblue darkcyan darkgoldenrod '
    'darkgray darkgrey darkgreen darkkhaki darkmagenta darkolivegreen darkorange '
    'darkorchid darkred darksalmon darkseagreen darkslateblue darkslategray darkslategrey '
    'darkturquoise darkviolet deeppink deepskyblue dimgray dimgrey dodgerblue firebrick '
    'floralwhite forestgreen fuchsia gainsboro ghostwhite gold goldenrod gray grey '
    'green greenyellow honeydew hotpink indianred indigo ivory khaki lavender '
    'lavenderblush lawngreen lemonchiffon lightblue lightcoral lightcyan '
    'lightgoldenrodyellow lightgray lightgrey lightgreen lightpink lightsalmon '
    'lightseagreen lightskyblue lightslategray lightslategrey lightsteelblue lightyellow '
    'lime limegreen linen magenta maroon mediumaquamarine mediumblue mediumorchid '
    'mediumpurple mediumseagreen mediumslateblue mediumspringgreen mediumturquoise '
    'mediumvioletred midnightblue mintcream mistyrose moccasin navajowhite navy '
    'oldlace olive olivedrab orange orangered orchid palegoldenrod palegreen '
    'paleturquoise palevioletred papayawhip peachpuff peru pink plum powderblue '
    'purple rebeccapurple red rosybrown royalblue saddlebrown salmon sandybrown '
    'seagreen seashell sienna silver skyblue slateblue slategray slategrey snow '
    'springgreen steelblue tan teal thistle tomato turquoise violet wheat white '
    'whitesmoke yellow yellowgreen').split())
# 색 리터럴은 #RGB·#ARGB·#RRGGBB·#AARRGGBB만이에요. Quick3D 기본 도형 이름("#Cube")처럼 '#'으로 시작하는 다른 문자열은 색이 아니에요.
HEX_COLOR = re.compile(r'#(?:[0-9a-fA-F]{3,4}|[0-9a-fA-F]{6}|[0-9a-fA-F]{8})')


def check(ctx):
    """테마 밖의 색과 전체 값이 숫자인 크기를 찾아요."""
    for path, text in ctx['files'].items():
        if not path.endswith('.qml') or path == 'src/platform/ui/Theme.qml' \
                or path.startswith('src/platform/ui/themes/'):
            continue
        for number, line in enumerate(text.splitlines(), 1):
            line = re.sub(r'//.*', '', line)
            for match in re.finditer(r'\b([\w.]+)\s*:\s*([^;{}]+)', line):
                if match[1].startswith('anchors.'):
                    continue
                name, value = match[1].split('.')[-1], match[2].strip()
                size = name in {'width', 'height', 'implicitWidth', 'implicitHeight',
                    'padding', 'margins', 'spacing', 'radius'} or name.endswith(('Padding', 'Margin'))
                if size and re.fullmatch(r'[+-]?(?:\d+(?:\.\d*)?|\.\d+)', value) and float(value):
                    yield path, number, '테마 밖의 고정 크기: ' + name
                strings = re.findall(r'[\"\']([^\"\']*)[\"\']', value)
                if any(HEX_COLOR.fullmatch(s) or s.lower() in COLORS for s in strings) \
                        or re.search(r'Qt\.(?:rgba|hsla|hsva)\s*\(', value):
                    yield path, number, '테마 밖의 색 리터럴'
