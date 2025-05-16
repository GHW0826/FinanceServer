# 역할   
- IOCP 기반 통신의 단위 객체   
- 소켓 수명과 입출력 처리를 관리하며, 연결/해제/수신/송신 이벤트를 처리.   
- ConnectEx, DisconnectEx, WSARecv, WSASend를 IOCP와 연동   

## 주요 멤버 구성   
```cpp
class Session : public IocpObject {
    SOCKET _socket;
    RecvBuffer _recvBuffer;
    Queue<SendBufferRef> _sendQueue;
    Atomic<bool> _connected;
    weak_ptr<Service> _service;
};
```
   
## 주요 메서드 설명   
### bool Connect()   
- 클라이언트에서 ConnectEx() 호출   
- 연결 성공 시 ProcessConnect() 호출 → OnConnected() + 수신 등록   
   
### void Disconnect()   
- DisconnectEx() 호출   
- ProcessDisconnect() → OnDisconnected() + 서비스에 세션 제거   
   
### void Send(SendBufferRef)   
- 큐에 데이터 삽입 후 RegisterSend() 실행   
- _sendRegistered 플래그를 통해 중복 전송 방지   
   
### void RegisterRecv()   
- WSARecv()로 비동기 수신 등록   
- 완료 시 ProcessRecv() → 버퍼 기록 후 OnRecv() 호출 → RegisterRecv() 재등록   
   
### void RegisterSend()   
- _sendQueue에서 버퍼들을 꺼내 WSASend() 호출   
- 완료 시 ProcessSend() → OnSend() 호출 → 재등록 여부 판단

## IOCP 이벤트 처리 흐름   
### Dispatch(IOCPEvent* e)   
- IOCP 완료 후 호출되는 진입점으로 이벤트 타입에 따라 분기 처리됨:
```cpp
switch (event->_eventType) {
  case Connect: ProcessConnect(); break;
  case Disconnect: ProcessDisconnect(); break;
  case Recv: ProcessRecv(bytes); break;
  case Send: ProcessSend(bytes); break;
}
```

## 연동 구조 흐름도 (Session 기반 통신 흐름)   
```
클라이언트 접속 흐름:

ClientService::Start()
        ↓
Session::Connect() → ConnectEx()
        ↓
IOCP 완료 → Dispatch(EventType::Connect)
        ↓
Session::ProcessConnect()
        ↓
Service::AddSession()
        ↓
Session::RegisterRecv() → WSARecv()


서버 수신 흐름:

데이터 수신 완료 (IOCP 완료)
        ↓
Dispatch(EventType::Recv)
        ↓
ProcessRecv()
    → RecvBuffer::OnWrite()
    → Session::OnRecv()
    → RecvBuffer::OnRead()
    → RecvBuffer::Clean()
        ↓
재등록: RegisterRecv()



데이터 송신 흐름:

Session::Send()
        ↓
SendBuffer → _sendQueue에 Push
        ↓
RegisterSend() → WSASend()
        ↓
IOCP 완료 → Dispatch(EventType::Send)
        ↓
ProcessSend()
    → OnSend()
    → 재등록 여부 판단
```

