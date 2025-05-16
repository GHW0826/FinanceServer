MemoryPool readme.md


MemoryPoolStudy/
├─ README.md                   // 최상위 요약 소개
├─ docs/
│  ├─ design_overview.md       // 설계/아이디어 문서
│  ├─ usage_instructions.md    // 사용법, 빌드 방법 등
│  └─ references.md            // 참고 자료, 링크 등
├─ src/
│  ├─ MemoryPool.h
│  ├─ MemoryPool.cpp
│  ├─ ObjectPool.h
│  ├─ ObjectPool.cpp
│  └─ ...
├─ examples/
│  ├─ Example_MemoryPool.cpp   // 간단 활용 예제
│  ├─ Example_ObjectPool.cpp   // 오브젝트 풀 활용 예제
│  └─ ...
├─ tests/
│  ├─ TestMemoryPool.cpp       // 단위 테스트 or 간단 벤치마크
│  └─ TestObjectPool.cpp
├─ CMakeLists.txt              // 빌드 시스템 (CMake 예시)
└─ LICENSE (optional)          

[allock](https://learn.microsoft.com/ko-kr/windows/win32/memory/comparing-memory-allocation-methods)
