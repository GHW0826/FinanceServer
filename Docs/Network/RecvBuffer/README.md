
# 역할   

- 세션마다 멤버 변수로 들고 있어, 비동기 소켓 수신 데이터를 버퍼에 누적하고, 읽기/쓰기 커서를 분리하여 관리 
- 패킷 파서가 안정적으로 데이터 처리할 수 있도록 지원

```cpp
class RecvBuffer
{
    int32 _capacity;         // 전체 버퍼 용량
    int32 _bufferSize;       // 1회당 예상 패킷 크기 단위
    int32 _readPos = 0;      // 서버에서 데이터를 읽을 위치
    int32 _writePos = 0;     // 소켓으로 수신된 데이터를 쓸 위치
    Vector<BYTE> _buffer;    // 내부 데이터 저장 공간
};
```

## 주요 메서드 설명   
### BYTE* ReadPos()   
- 현재 읽을 데이터의 시작 주소를 반환함   
### BYTE* WritePos()   
- 수신 데이터가 저장될 위치를 반환함   
### bool OnRead(int32)   
- 지정한 바이트 수만큼 읽기 커서 이동 (패킷 처리 후 호출)   
### bool OnWrite(int32)   
- 수신 완료된 바이트 수만큼 쓰기 커서 이동
### void Clean()   
- 버퍼 정리 함수   
- 데이터가 다 읽힌 경우 커서 초기화   
- 데이터 남은 상태에서 Free 공간이 부족하면 데이터를 앞으로 복사해 커서 정렬
   
   
## 동작 흐름도   
1 WSARecv() 완료 후 → WritePos()에 데이터 수신   
2 수신 길이만큼 → OnWrite(n) 호출   
3 파서가 처리한 패킷 크기만큼 → OnRead(n) 호출   
4 여유 공간 부족 시 → Clean() 호출로 버퍼 리팩토링   
