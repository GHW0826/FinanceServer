# 역할   
- MemoryPool을 기반으로 하는 템플릿 기반 객체 풀   
- T 타입 객체를 반복 생성/파괴하지 않고 재사용하여 성능 최적화   
- Job, TimerItem, SendBuffer 등 고빈도 생성되는 객체에 사용됨
- SPool: 내부적으로 SLIST 기반 MemoryPool을 타입별로 고정 크기로 보유   
- SAllocSize: T 객체 + MemoryHeader 포함 크기

## 주요 메서드 설명   
### T* Pop(Args&&...)   
- MemoryPool로부터 메모리 확보 → AttachHeader()로 데이터 포인터 반환   
- placement new로 생성자 호출   
- StompAllocator 사용 조건 분기 가능 (#ifdef _STOMP)   
   
### void Push(T*)   
- 객체 소멸자 호출   
- DetachHeader() → MemoryPool에 메모리 반납
   
### shared_ptr<T> MakeShared(...)   
- Pop()으로 생성한 객체를 shared_ptr로 래핑   
- 커스텀 deleter로 Push() 사용 → shared_ptr이 소멸되면 자동 반환   
   
## 내부 동작 요약   
```
[Pop()] → MemoryPool.Pop() + new(...) 생성 → 사용자 코드에서 사용
[Push()] → 소멸자 호출 → MemoryPool.Push()
[MakeShared()] → shared_ptr<T>(Pop(), Push) 반환
```
