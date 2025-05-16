# 역할   
- SOCKADDR_IN을 캡슐화해 네트워크 주소(IP + Port)를 관리   
- 소켓 연결 시 사용되는 주소 구성/조회/변환을 담당   
- 문자열 IP ↔ IN_ADDR 간 변환을 제공

## 주요 생성자   
### NetAddress(SOCKADDR_IN)   
- 외부에서 받은 소켓 주소 구조체를 래핑   
### NetAddress(wstring ip, uint16 port)   
- 문자열 IP 주소와 포트를 받아 내부 _sockAddr 구성   
- 내부적으로 InetPtonW() 사용하여 문자열 IP를 IN_ADDR로 변환

## 주요 메서드 설명   
### SOCKADDR_IN& GetSockAddr()   
- 내부 소켓 주소 구조체 반환 (bind, connect 등에 사용)   
   
### uint16 GetPort()   
- 포트를 반환 (ntohs()로 네트워크 바이트 순서에서 호스트 순서로 변환)   
   
### wstring GetIpAddress()   
- IP를 문자열 형태로 반환 (InetNtopW() 사용)   
    
### static IN_ADDR Ip2Address(const WCHAR* ip)   
- 문자열 IP를 IN_ADDR로 변환 (InetPtonW 내부 사용)   
   
## 핵심 함수 설명   
### InetPtonW(AF_INET, ipStr, &addr)   
- IP 문자열 (예: "192.168.0.1")을 이진 형식 IN_ADDR로 변환   
- AF_INET: IPv4 지정   
- 실패 시 반환값 0   
   
### InetNtopW(AF_INET, &addr, buffer, size)   
- IN_ADDR를 문자열 IP로 역변환   
- IP 디버깅 출력 등에 사용됨   
