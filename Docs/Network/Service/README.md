# 역할   
- Service 클래스 계열은 서버와 클라이언트의 공통 세션 관리/생성 인터페이스를 담당.   
- Service: 공통 베이스 클래스   
- ClientService: 클라이언트 입장에서 Connect() 실행   
- ServerService: 서버 입장에서 Accept() 대기

## 주요 동작 방식   
### 공통 기능 (Service)   
- CreateSession() → SessionFactory()로 세션 생성 후 IOCP 등록   
- AddSession(), ReleaseSession()으로 세션 수 관리   
- Broadcast()로 전체 세션에 전송   

### 서버 (ServerService)   
- Listener를 통해 AcceptEx 등록 → 세션을 Accept하면 AddSession   
   
### 클라이언트 (ClientService)   
- Connect() 호출로 서버 접속 시도 → 연결 성공 시 AddSession

### 실행 흐름 요약
```
[ServerService::Start()]           [ClientService::Start()]
         ↓                                 ↓
  Listener::StartAccept()           Session::Connect()
         ↓                                 ↓
    Accept() 완료                        연결 완료
         ↓                                 ↓
     AddSession()                        AddSession()
         ↓                                 ↓
     IOCP 등록 완료 → 통신 시작
```
