# cpp_server
- IOCP 기반의 C++ 게임 서버 구현을 목표.

## 📁 디렉토리 설명
### 🚀 [cpp_server](./Server)   
- 실제 C++ 서버 구현 디렉토리.   
- IOCP 네트워크 통신, 세션 관리, 디스패처, 버퍼 관리, 패킷 처리 등 코드들이 들어있습니다.
   
### 📘 [Docs](./Docs)
- cpp_server의 핵심 클래스와 설계 구조를 문서화한 디렉토리입니다.   
- `Session`, `Service`, `IocpCore`, `Buffer` 등 서버 구성 요소별 역할과 관계 설명이 포함됩니다.   
- 구조 이해나 커스터마이징을 위한 내부 문서로 활용됩니다.   
   
### 🔧 [Benchmark](./Benchmark)   
- 서버의 성능을 측정하고 분석하는 벤치마크 코드 모음.   
- 요청 처리 속도, 응답 시간, 처리량(TPS) 등을 측정하는 테스트들이 포함.


### 🧩 [Snipet](./Snipet)   
- 테스트용 예제 및 실험적 코드 스니펫들이 담겨 있는 디렉토리.   
- 메모리 풀, 락, 스레드 동기화 등 개별 기능의 독립적인 검증 목적.   
- 기능 구현 실험용으로 사용.   

## Dependencies
- [Boost 1.86](https://www.boost.org/) (Asio, Thread)
- [LeksysINI](https://github.com/Lek-sys/LeksysINI) (ini load for config)
- [Quill](https://github.com/odygrd/quill) (Log Lib)
