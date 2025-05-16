overview.md 

- example

- 2.1. 학습 목적
메모리 풀(Memory Pool)이란, 메모리 할당/해제가 빈번한 상황에서, 동적 할당 오버헤드를 줄이기 위해 미리 큰 블록을 확보해두고 필요할 때 블록을 나눠서 재사용하는 기법이다.
특히 게임 서버나 실시간 시스템에서는 CPU 캐시, 락 경합 최소화를 위해 메모리 풀을 자주 사용한다.

2.2. 구현 방식 요약
A) Lock-Free MemoryPool (Windows)
Interlocked SList 사용

Windows API InitializeSListHead, InterlockedPushEntrySList, InterlockedPopEntrySList를 통해 단일 연결 리스트를 락 없이 다룰 수 있다.
_aligned_malloc / _aligned_free를 조합해 정렬된 메모리를 얻고, ABA 문제는 SList가 내부적으로 해결해준다.
헤더 구조

MemoryHeader가 SLIST_ENTRY를 멤버(혹은 상속)로 가지고, 그 뒤에 실제 데이터가 따라붙는 구조.
Push할 때 MemoryHeader의 allocSize = 0으로 세팅. Pop 시 새로 할당하거나 재사용.
통계용 Atomic

_allocCount 같은 원자적 카운터로, 현재 풀에서 몇 개의 블록이 사용 중인지 추적 가능.
B) Lock-Based MemoryPool (Mutex)
std::mutex + std::queue
하나의 전역(or 싱글턴) 풀에서 std::queue<MemoryHeader*>로 재사용 블록을 관리
Push/Pop 시 std::unique_lock<std::mutex>로 보호
C) ObjectPool
객체 타입(예: class Monster)에 대한 풀을 템플릿 기반(template<typename T> class ObjectPool)으로 구현 가능
객체 생성/소멸 비용도 줄이기 위해, placement new, ~T() 호출을 컨트롤
2.3. 성능 측정(벤치마크 결과)
항목	Lock-free Pool	Lock-based Pool
Push/Pop (1 thread)	~5 ns/ops	~8 ns/ops
Push/Pop (4 threads)	~15 ns/ops	~40 ns/ops
(가상의 예시 수치)

Lock-free가 다중 스레드 상황에서 더 좋은 확장성 제공.
단, 구조가 조금 더 복잡하고, Windows 전용 Interlocked API 의존.
