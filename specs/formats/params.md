# 파라미터 파일 (`features/params` 소유)

한 읽기·쓰기 코드 `ParamFile`(순수)만 이 형식을 다룬다. 백업도 같은 코드로 쓴다.

## 쓰기 (`.params`, UTF-8, 줄 끝 `\n`)
```
# Onboard parameters for Vehicle <sysid>
#
# Stack: <firmwareName>
# Vehicle: <vehicleType>
# Version: <firmwareVersion>
#
# Vehicle-Id	Component-Id	Name	Value	Type
<sysid>	1	BAT1_N_CELLS	4	6
<sysid>	1	BAT1_V_CHARGED	4.05	9
```
- 구분은 탭. Type은 `MAV_PARAM_TYPE` 번호. 이름 자연순.
- 값은 FC 값이다. 대기값은 넣지 않는다.
- 정수형은 정수로, 실수형은 `float`가 왕복하는 가장 짧은 십진 표기(`formatValue`)로 쓴다.

## 읽기 (`.params` `.param` `.parm` 공통)
- 빈 줄과 `#`으로 시작하는 줄은 건너뛴다.
- 탭으로 나눠 다섯 칸이고 1·2·5칸이 숫자면 QGC 줄: 이름은 3칸, 값은 4칸. 2칸(Component-Id)이 1이 아니면 그 줄은 버리고 `otherComponent`에 센다.
- 그 밖에는 쉼표·공백·탭(`[,\s]+`)으로 나눠 앞 두 칸을 이름과 값으로 읽는다(Mission Planner 2열). 셋째 칸부터는 버린다.
- 값이 숫자가 아니거나 이름이 비면 `badLines`에 센다.
- 같은 이름이 또 나오면 뒤 값을 쓴다.
- 읽은 항목이 0개면 `ParamError::FileEmpty`, 파일을 못 열면 `ParamError::FileOpen`.
