# 역할   
- SLIST_ENTRY 기반의 lock-free 메모리 풀 구현체   
- 동일 크기의 블록을 반복적으로 할당/반환하며, malloc/free보다 빠른 성능 제공.   
- SendBuffer, Job, JobData, TimerItem 등 빈번한 메모리 생성/파괴가 필요한 곳에서 사용됨.   

## SLIST 기반 구조   
- SLIST_HEADER + SLIST_ENTRY로 구성된 Windows 전용 lock-free 스택   
- ::InterlockedPushEntrySList() / ::InterlockedPopEntrySList() 사용   
- 내부적으로 CAS(Compare And Swap)를 기반으로 동기화 없이 작동   

## 주요 메서드 설명   
### MemoryHeader* Pop()   
- 스택에서 블록을 하나 꺼냄   
- 없다면 _aligned_malloc() 으로 새로 할당   
- 디버깅용 allocSize는 0이 되어야 정상 반환된 상태   
   
### void Push(MemoryHeader* ptr)   
- 반환된 블록을 _header 스택에 다시 push   
- allocSize를 0으로 초기화 후 반환   
   
### void* AttachHeader() / DetachHeader()   
- 사용자 데이터와 MemoryHeader 간 경계를 추적   
- 헤더 앞붙이기 → Data = Header + 1 방식   
- 할당 시에는 AttachHeader, 해제 시에는 DetachHeader 호출   

### SLIST_ENTRY 기반 lock-free 구조 요약   
```
초기 상태:
[SHEAD]
  ↓
[SLIST1] → [SLIST2] → [SLIST3]

Pop 1회:
[SHEAD]
  ↓
[SLIST2] → [SLIST3]

Pop 1회:
[SHEAD]
  ↓
[SLIST3]

Push 1회 (SLIST4):
[SHEAD]
  ↓
[SLIST4] → [SLIST3]
```
```
[SLIST_HEADER] → [MemoryHeader][Data] → [MemoryHeader][Data] → ...   
                          ▲   
                        사용자가 받는 포인터 = Header + 1
```
- Push: ::InterlockedPushEntrySList(&_header, ptr)   
- Pop : ::InterlockedPopEntrySList(&_header)   

- [SLIST_ENTRY Docs1](https://learn.microsoft.com/ko-kr/windows/win32/api/winnt/ns-winnt-slist_entry)   
- [SLIST_ENTRY Docs2](https://learn.microsoft.com/ko-kr/windows/win32/sync/interlocked-singly-linked-lists?redirectedfrom=MSDN)   
- [SLIST_ENTRY Docs3](https://learn.microsoft.com/ko-kr/windows/win32/sync/using-singly-linked-lists)   
