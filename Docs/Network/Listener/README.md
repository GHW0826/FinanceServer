# 역할   
- 서버 소켓을 관리하고 AcceptEx 기반 비동기 클라이언트 접속 수락을 담당   
- ServerService에서 클라이언트 연결을 받아오기 위한 핵심 컴포넌트   
- Listener는 IOCP에 등록되어 Accept 이벤트를 처리.   


##  주요 메서드 설명   
### bool StartAccept(ServerServiceRef service)   
- 새 소켓 생성 및 IOCP 등록   
- 소켓 설정 (SO_REUSEADDR, SO_LINGER) 후 Bind + Listen   
- 최대 세션 수만큼 AcceptEvent를 준비하고 AcceptEx 등록   
   
### void RegisterAccept(AcceptEvent* acceptEvent)   
- 새로운 Session을 만들고 AcceptEx 호출로 비동기 클라이언트 접속 대기 등록   
   
### void ProcessAccept(AcceptEvent* acceptEvent)   
- Accept 완료 시 호출   
- 연결된 클라이언트 소켓 정보 설정 (SetUpdateAcceptSocket, getpeername)   
- Session::ProcessConnect() 호출해 연결 처리 시작   
- 즉시 다음 AcceptEx 등록 재시도   
   
### void CloseSocket()   
- 리스너 소켓 종료 및 INVALID_SOCKET 처리   
   
### Dispatch(IOCPEvent* event)   
- IOCP 완료 시 호출되어 Accept 이벤트를 분기 처리   
