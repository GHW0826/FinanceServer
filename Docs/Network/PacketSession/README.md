# 역할   
- IOCP 기반 Session의 확장 버전, 고정된 패킷 형식(헤더 + 바디)을 수신 처리하는 클래스   
- OnRecv()에서 완성된 패킷 단위로 추출 후 OnRecvPacket()에 위임하는 구조   
- 가장 일반적인 패킷 처리 기반 세션의 베이스 클래스

## 패킷 처리 흐름   
### 수신 처리 (OnRecv)   
```cpp
while (true) {
    if (남은 데이터 < 헤더)
      break;
    PacketHeader header = *(PacketHeader*)&buffer[pos];
    if (남은 데이터 < header.size)
      break;
    OnRecvPacket(buffer + pos, header.size);
    pos += header.size;
}
```
- 최소 PacketHeader만큼의 데이터가 없으면 루프 중단   
- header.size만큼 수신된 패킷 단위로 OnRecvPacket() 호출   

### 사용자 정의 처리 (OnRecvPacket)   
- 실제 패킷 핸들링은 이 순수 가상 함수(abstract)를 통해 개별 세션에서 처리   
