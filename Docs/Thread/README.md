
# 개요   
- Thread/ 디렉토리는 로직 처리용 스레드 요소들을 모아둠. LogicThread, TimerThread 등을 구동하며, 작업 분배 및 예약 실행.   
   
## 구성 요소 요약
| 파일명 | 설명 | 
|--------|------|
| [ThreadPool](./ThreadPool) | LogicWorker 및 기타 스레드를 관리하고, GlobalQueue, JobTimer 기반 루프 실행 |
   
## 사용 방식   
- 서버 실행 시 ThreadPool::Launch()으로 다수의 LogicThread 구동   
- 각 스레드는 내부적으로 DoGlobalQueueWork() + DistributeReservedJobs() 루프 실행   
- JobQueue 기반 순차 처리와 Timer 기반 지연 실행을 함께 처리.   

## 흐름 요약   
[ThreadPool::Launch()] → LogicThread 생성 → GlobalQueue.Pop() → JobQueue.Execute() or JobTimer.Distribute(now)   
