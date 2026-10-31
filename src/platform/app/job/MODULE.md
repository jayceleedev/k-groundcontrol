# app/job (바탕 0층)
역할: 오래 걸리는 작업의 상태·단계·진행률·끝맺음을 한 모양으로 내놓아요.
수명: 작업마다 하나, 부모는 만든 모듈이에요.
받는 것: 작업을 돌리는 모듈의 setStep·setProgress·succeed·fail·finishCancelled 호출.
주는 것: Job, Job::Releaser, OperationKind, JobError, errorText.
기여: 없음.
의존: app/errors, app/log.
안 하는 것: 실제 작업을 돌리지 않고, 시동 감시는 vehicle/safety가, 화면은 platform/ui의 JobView가 맡아요.
예외: OperationKind는 기능 이름이 들어 있는 이름표 열거예요(BASE §9.4). 코드 의존은 아니에요.
