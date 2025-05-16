# 역할   
- 패킷 버퍼 내에서 데이터를 안전하게 읽어들이기 위한 유틸리티 클래스   
- Peek()은 읽기만 수행, Read()는 읽고 커서 이동, operator>>는 간편 추출 지원   
- RecvBuffer나 PacketSession에서 패킷 헤더 및 바디 파싱용으로 사용됨

| 함수 | 설명| 
|------|-----|
| Peek(dest, len) | 현재 위치에서 복사만 수행 (커서 이동 없음) |
| Read(dest, len) | Peek 후 커서를 len 만큼 이동 |
| operator>>(T&) | 지정 타입으로 변환하여 커서 이동 후 리턴 |
| FreeSize() | 남은 읽기 가능한 바이트 수 반환 |
