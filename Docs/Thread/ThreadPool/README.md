# 역할   
- 스레드를 동적으로 생성하고, 로직 처리 또는 예약 작업 처리에 사용되는 스레드 풀 컨트롤러   
- cpp_server 프로젝트에서는 LogicWorker, JobTimer 등을 구동하는 데 사용.  
- 각 스레드는 InitTLS()로 TLS(Thread Local Storage)를 초기화하여 고유 스레드 ID를 갖게 됨.


## 주요 메서드 설명   
### void Launch(function<void()> callback)   
- 새로운 스레드를 생성하고, 지정된 callback 함수를 실행함   
- 생성된 스레드는 InitTLS()로 TLS 초기화 후, DestroyTLS()로 정리   
- 내부 _threads 벡터에 스레드를 저장하여 추후 Join() 가능
   
### void Join()   
- 모든 스레드를 join()으로 종료 대기  
- 스레드 풀 종료 시 자원 정리
   
## 정적 함수 설명 (스레드 내 작업)   
### static void InitTLS()   
- 전역 Atomic 카운터로부터 고유한 LThreadId 설정   
- 스레드마다 고유 ID 부여 (디버깅, 로깅, 스레드 분기 등에 활용 가능)   
   
### static void DoGlobalQueueWork()   
- 현재 스레드에서 실행 가능한 JobQueue를 GGlobalQueue에서 꺼내 순차적으로 Execute()   
- 현재 tick (LEndTickCount) 기준으로 일정 시간까지 루프 실행   
   
### static void DistributeReservedJobs()   
- 현재 시간 기준으로 실행 예정인 Job을 GJobTimer->Distribute(now) 호출해 분배   
