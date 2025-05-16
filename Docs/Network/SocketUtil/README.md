# 역할   
- ConnectEx, DisconnectEx, AcceptEx 와 같은 확장 Windows 소켓 함수 로딩 및 보조 함수 제공   
- Session, Listener, IOCPCore 등에서 소켓을 생성하고 각종 설정을 적용할 때 사용됨

## 주요 메서드 설명   
### void Init()   
- WSAStartup 수행 후 ConnectEx, DisconnectEx, AcceptEx 함수 포인터를 런타임에 로딩함   
- WSAIoctl + SIO_GET_EXTENSION_FUNCTION_POINTER 사용   
   
### void Clear()   
- WSACleanup() 호출   
   
### SOCKET CreateSocket()   
- WSASocket 호출로 WSA_FLAG_OVERLAPPED 옵션의 소켓을 생성
   
### bool Bind(SOCKET, NetAddress) / BindAnyAddress()   
- bind() 시스템 콜로 소켓에 주소 바인딩    
- INADDR_ANY 주소를 바인딩하는 서버측 유틸리티   
   
### bool Listen(SOCKET, int32 backlog)   
- 대기 큐 사이즈와 함께 listen() 호출   
   
### void Close(SOCKET&)   
- 유효한 소켓이면 closesocket() 호출 후 INVALID_SOCKET으로 초기화   
   
### bool SetReuseAddress() / SetRecvBufferSize() / SetSendBufferSize() / SetNoDelay() 등   
- setsockopt()을 감싼 템플릿 기반 설정 유틸리티   
   
### bool SetUpdateAcceptSocket()   
- Accept된 소켓에 대해 SO_UPDATE_ACCEPT_CONTEXT 설정 (AcceptEx 사용 시 필수)   
   
### bool BindWindowsFunction()   
- 소켓 핸들에 대해 WSAIoctl을 사용해 GUID 기반 함수 포인터 바인딩   
