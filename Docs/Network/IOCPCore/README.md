
# 역할
- Windows IOCP (I/O Completion Port) 기반 비동기 소켓 처리를 위한 메인 클래스   
- 비동기 작업이 완료되었을 때 이벤트를 수신하고, 해당 객체의 Dispatch() 메서드를 호출


# 주요 메서드 설명
## IOCPCore() (생성자)
- IOCP 포트를 생성 (CreateIoCompletionPort(INVALID_HANDLE_VALUE, ...))
   
## ~IOCPCore() (소멸자)
- 핸들 해제 (CloseHandle) 
   
## HANDLE GetHandle()
- 내부 IOCP 핸들을 외부에서 가져올 수 있도록 노출
   
## bool Register(IocpObjectRef object)
- 소켓 핸들을 IOCP 포트에 연결 (바인딩)
- 각 Session, Listener 등 IocpObject 기반 객체들이 자신을 등록할 때 사용
   
## bool Dispatch(uint32 timeoutMs = INFINITE)
- GetQueuedCompletionStatus() 호출하여 완료 이벤트 수신
- 비동기 I/O 작업이 완료되었을 때 그 결과를 가져옴.
- IOCP 큐에 쌓인 완료 이벤트를 하나 꺼내서 반환함
- 이때, 이벤트 하나당 하나의 워커 스레드만 깨어나고 처리함
- IOCP에서 한 번의 이벤트는 반드시 OVERLAPPED 기반 구조와 연결됨
- IocpEvent는 _owner를 통해 어떤 객체에서 발생했는지를 추적함
- Dispatch()는 수신, 송신, 접속 등 타입별 분기로 이어짐
- 성공 시: 완료된 IocpEvent에서 _owner 추출하여 Dispatch() 호출
- 실패 시: errCode에 따라 로깅하거나 디버깅 처리, Dispatch()는 여전히 호출됨 (예: 연결 종료 처리 포함)
   

## 동작 흐름도
- IOCP 워커 스레드가 Dispatch() 호출하며 대기
- 소켓 수신/송신 완료 시, 이벤트가 IOCP 큐에 등록됨
- GetQueuedCompletionStatus()가 이벤트 꺼냄
- 해당 IocpObject::Dispatch(event) 호출
- IOCP 스레드는 다음 이벤트 대기
