# params (기능 순위 8)
역할: FC 파라미터 전체를 받아 보여 주고, 고쳐 쓰고, 파일로 옮겨요.
수명: 혼합(전역 ParamRebootReconnect + 기체별 ParamService). Composition이 전역 하나를 만들고, vehicleAdded에서 ParamService를 만들어요. 서비스의 부모는 Vehicle이에요. 컨트롤러는 전역이고 activeChanged로 다시 묶어요.
받는 것: LinkManager의 linkClosed·linkFailed, VehicleRegistry.vehicleAdded, Vehicle.messageReceived의 PARAM_VALUE, VehicleStates의 link·arming·capabilities, FirmwarePlugin의 paramEncoding·paramMetadataSource·paramGroup·paramGroupTitle·paramFileSkip, ArmedPolicy 판정.
주는 것: ParamRebootReconnect(watch), ParamService(of·params·downloadState·metadataState·job·rebootNeeded·receivedCount·totalCount·refresh·setPending·clearPending·writeDecision·writePending·saveFile·compareFile·applyFile·reboot), Params 묶음, ParamMeta, Param, FileDiff, ParamStates(DownloadState·MetadataState), ParamError, errorText, registerUi.
기여: 좌 메뉴 14번 수동 기체 설정 패널(묶음 4).
의존: app/errors, app/job, app/log, app/net, app/paths, app/scheduler, comm/links, comm/mavlink, vehicle/registry, vehicle/firmware, vehicle/safety, vehicle/command, vehicle/outbound, ui.
안 하는 것: MAVLink FTP, PX4 해시 캐시, 즐겨찾기, 공장 초기화, RC 연결, 다른 컴포넌트의 파라미터, 기체 없이 파일 보기, 백업 자동 삭제는 하지 않아요. 범위 밖 값을 물어보는 일은 화면이 해요. 토스트는 notify 모듈이 생길 때까지 패널 안에 있어요.
흐름(내려받기):
1. ParamService 생성·링크 복귀 → autoRefresh — 시동 중이면 SkippedArmed로 두고 아니면 받아요.
2. ParamDownload::start — PARAM_REQUEST_LIST를 보내요.
3. ParamService::receive — PARAM_VALUE를 풀어 Params에 넣어요.
4. ParamDownload — 1초 끊기면 빠진 번호를 PARAM_REQUEST_READ로 다시 받아요.
흐름(쓰기):
1. ParamsController.writeCheck → ParamService::writeDecision — 시동 중 규칙을 봐요.
2. ParamsController.write → ParamService::writePending — 전체 값을 백업해요.
3. ParamWrite — 이름순으로 PARAM_SET을 보내고 돌아온 값을 확인해요.
4. ParamService::writeFinished — 결과 토스트를 내요.
흐름(파일):
1. ParamsController.openFile → ParamService::compareFile — 파일을 FC 값과 비교해요.
2. ParamsController.applyFile → ParamService::applyFile — 고른 값을 대기값으로 올려요.
재부팅 뒤 다시 잇기: 재부팅이 성공으로 판정되면 ParamRebootReconnect.watch가 30초 창을 열어요. 창 안에서 링크가 오류로 끝나면 6초 뒤 LinkManager::connectLink로 같은 연결을 열고, 실패하면 1초마다 다시 해요. 새 기체, 사용자 끊기, 창 끝에서 멈춰요.
메타데이터: capabilities가 찬 뒤 한 번, ParamMetadataLoader가 캐시를 먼저 보고 없으면 주소를 차례로 내려받아요.
