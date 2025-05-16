
# Allocator 정리   
| Allocator | 목적 | 요약 |
|-----------|------|----------|
| BaseAllocator | 기본 malloc/free 래퍼 |  표준 C 런타임 사용 |
| StompAllocator |  버퍼 오버런 탐지용 디버깅 할당기 |  페이지 단위 예약 + 경계 조정(스톰핑) |
| PoolAllocator |  고성능 재사용 풀 |  프로젝트의 GMemory 풀 인터페이스 |
| StlAllocator |  STL 컨테이너용 커스텀 할당기 |  PoolAllocator를 STL 호환 래퍼로 제공 |

## BaseAllocator   
```cpp
void* Alloc(int32 size)  { return ::malloc(size); }
void  Release(void* ptr) { ::free(ptr); }
```
- 가장 단순한 래퍼.   
- 플랫폼 이식성 확보에 유용하지만, 성능/디버깅 기능은 없음.   
   

## StompAllocator (디버그 & 오버런 탐지)   
### 동작 방식 [VirautlAlloc](./VirtualAlloc.md)   
- “Guard Page + 페이지 경계 정렬” 로 메모리 끝을 보호해 오버런 / 언더런을 즉시 Access Violation 으로 잡아내는 할당기.   
- 페이지 단위(0x1000 = 4KB) 로 크기 반올림 → pageCount 계산.   
- VirtualAlloc(MEM_RESERVE|MEM_COMMIT) 으로 pageCount * PAGE_SIZE 만큼 예약·커밋.   
- 오프셋 계산: dataOffset = pageCount*PAGE_SIZE - size → 반환 포인터를 마지막 페이지 끝쪽으로 이동.
- 해제 시, 포인터를 페이지 경계로 내림(baseAddress = ptr & ~(PAGE_SIZE-1)), VirtualFree.

### 사용 이유
- 버퍼 끝 직후가 페이지 경계가 됨.   
- 만약 오버런이 발생해 페이지를 넘어서면 다음 페이지가 아직 커밋되지 않았으므로 Access Violation 발생 → 즉시 디버깅 가능.   
- 생산 빌드에서는 성능 저하가 커서 보통 디버그 전용.   
- 실제 상용 Stomp 할당기는 ‘Guard Page (PAGE_NOACCESS)’를 한 장 더 두어 즉시 크래시를 보장. 이건 간단 용.

### Access Violation
- Guard Page 는 페이지 테이블에 ‘읽기/쓰기 불가’ 로 마킹되어 있음.   
- 버퍼가 끝을 넘는 순간 → CPU MMU(메모리 관리 장치) 가 페이지 권한을 체크 → 즉시 0xC0000005 (Access Violation) 예외 발생.   
- 디버거/Crash Dump 에서 오버런 지점과 스택트레이스를 바로 확인 가능 ➜ 버그 탐지 ✔️.   

### 장점   
- 오버/언더런 즉시 탐지
- 메모리 덤프 시 패턴 확인 용이   
### 단점   
- 할당·해제 비용 ↑ (VirtualAlloc 호출)   
- 메모리 공간 낭비 (page 단위)   
   
## PoolAllocator 
```cpp
void* Alloc(int32 size)   { return GMemory->Allocate(size); }
void  Release(void* ptr)  { GMemory->Release(ptr); }
```
- GMemory 는 메모리 풀 매니저 전역 인스턴스.   
- 미리 할당된 메모리를 재사용해 malloc/free 대비 수십 배 빠른 할당 속도.   
- 단편화 감소.   
   
## StlAllocator   
- PoolAllocator 를 STL 컴플라이언트 형태로 감싼 템플릿.   
- vector<int, StlAllocator<int>> vec; 처럼 사용 가능.   
- STL 컨테이너에서도 풀의 이점을 누릴 수 있음.   
