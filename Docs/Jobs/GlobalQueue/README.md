<img
  src="../../img/globalqueue.drawio.png"
  width="900"
  height="150"
/>   
# 역할   
- JobQueue(작업 큐)를 전역적으로 모아서 관리하는 글로벌 큐   
- 로직 처리 스레드(ThreadManager 등)가 작업할 JobQueue가 있는지를 확인할 때 사용   
- 실행 대기 중인 JobQueue가 있다면 로직 스레드가 해당 JobQueue를 실행하는 식으로 처리.
- 내부적으로 LockQueue를 사용하여 멀티스레드 환경에서도 안정성 확보   
- Push()는 외부 로직에서 처리 요청이 생긴 JobQueue를 등록   
- Pop()은 LogicWorker(Thread) 측에서 큐를 꺼내어 처리 시작

## 주요 메서드 설명   
### void Push(JobQueueRef jobQueue)   
- 작업할 Job이 존재하는 JobQueue를 글로벌 큐에 등록함
- 중복 방지 로직은 JobQueue 측에서 관리하는 구조가 일반적 (예: _registered 플래그)   
   
### JobQueueRef Pop()   
- 등록된 JobQueue 하나를 꺼냄   
- LogicThread는 이를 통해 Job이 있는 JobQueue만 처리

### 흐름 요약   
[Job 등록] → JobQueue::Push() → GlobalQueue::Push(JobQueue) → LogicThread가 JobQueue Pop() 후 Execute()   
