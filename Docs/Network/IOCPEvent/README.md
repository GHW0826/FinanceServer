# 역할   
- OVERLAPPED를 상속한 IOCP 이벤트 객체   
- WSARecv, WSASend, AcceptEx, ConnectEx 등 비동기 API 사용 시 넘겨지는 이벤트 구조체   
- _eventType과 _owner를 통해 어떤 종류의 이벤트이고, 어떤 객체(Session 등)에서 발생했는지를 추적
   
# 주요 함수
## Init() : OVERLAPPED 필드 초기화
## 생성자에서 _eventType 설정 + 초기화 수행
- 파생 클래스로 이벤트 타입 설정.

# EventType 종류
```cpp
enum class EventType : uint8 {
    Connect,
    Disconnect,
    Accept,
    Recv,
    Send
};
```

## 흐름도 (IOCP 내부 처리)
```
[WSARecv()] ──▶ [OVERLAPPED 포인터] → [IOCP 큐 등록]
                            │
            [GetQueuedCompletionStatus()]
                            ▼
            [IOCPEvent::_owner->Dispatch()]
```
