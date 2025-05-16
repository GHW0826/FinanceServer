# IOCP

**IOCP (I/O Completion Port)** 는 윈도우에서 **비동기** 방식의 소켓 통신(또는 파일 I/O)을 효율적으로 처리하기 위한 매커니즘.

1. **비동기 Overlapped I/O**:
   - 소켓 혹은 파일에 대해 Overlapped(비동기) 함수를 호출(예: `WSARecv`, `WSASend`, `AcceptEx`)하면, 즉시 반환하고 실제 I/O 완료 시점에 알림을 받음.
2. **Completion Port**:
   - 여러 소켓(핸들)을 하나의 IOCP에 연결(Associate)할 수 있다.
   - I/O 완료 시 `GetQueuedCompletionStatus()`를 통해 이벤트(Completion Packet)를 수신.
3. **고성능**:
   - 적은 수(CPU 코어 수 정도)의 **워커 스레드**가 I/O 완료 시점마다 작업을 처리하므로, 다수 클라이언트에 대응해도 스레드가 과도하게 늘어나지 않는다.
