# 개요   
- Memory/ 디렉토리는 cpp_server의 고성능 메모리 관리 요소들을 모아둠.
- 동적 할당 최소화, 메모리 재사용, 할당 추적 및 오버런 감지 등을 위해 다양한 풀과 할당기를 포함.

## 구성 요소 요약
| 파일명 | 설명 | 
|--------|------|
| [MemoryPool](./MemoryPool) | SLIST 기반의 고정 크기 메모리 블록 재사용 풀 (lock-free) |
| [ObjectPool<T>](./ObjectPool) | 템플릿 기반 객체 풀. placement-new와 custom deleter를 통한 객체 재사용 |
| [Allocator](./Allocator) | StompAllocator, PoolAllocator, StlAllocator 등 다양한 목적의 할당기 모음 |

## 각 요소 간 관계 및 역할
```
      [ObjectPool<T>]
            ↓
      [MemoryPool] ←───────────────┐
            ↑                      │
     [StlAllocator<T>] ←───── [PoolAllocator]
            ↓                      ↑
      [vector<T, Alloc>]      GMemory(global)
```
## 사용 방식   
- ObjectPool<T>::MakeShared()로 객체 생성 시 내부적으로 MemoryPool에서 메모리를 꺼냄.   
- AttachHeader()를 통해 사용자에게 전달할 객체 영역을 반환.   
- 사용 완료 후 shared_ptr 소멸자 또는 수동 Push() 호출로 다시 MemoryPool에 반환.   
- MemoryPool은 SLIST를 이용해 lock-free 방식으로 재사용 처리.   

## 흐름 요약   
```
ObjectPool<Job>::MakeShared()   // 객체 생성 요청
        ↓
MemoryPool::Pop()               // SLIST에서 블록 가져오기 (없으면 malloc)
        ↓
AttachHeader → placement-new   // 객체 생성
        ↓
사용 (예: JobQueue에 Push)
        ↓
shared_ptr 소멸 or 직접 Push() 호출
        ↓
~Job() 호출 → DetachHeader → MemoryPool::Push() // 메모리 반환
```
