
# Lock
  
## 1. 개요
멀티스레드 환경에서 공유 자원(예: 전역 변수, 동적 메모리, 파일 등)에 여러 스레드가 동시에 접근하면, 데이터 무결성이 깨지는 현상이 발생할 수 있습니다.   
이를 방지하기 위해 한 번에 한 스레드만 임계영역(critical section)을 수행하도록 동기화가 필요한데, 그 대표적 방법이 락(Lock)을 사용하는 것   

### 1. std::mutex   
C++11 이후 표준으로 제공되는 동기화 객체.   
내부적으로 대부분 OS 커널의 뮤텍스/세마포어/CRITICAL_SECTION 등을 래핑해 구현.   
충돌 시 해당 스레드는 블록되어 CPU를 소모하지 않음. 다른 스레드가 unlock()하기 전까지 OS 스케줄러 대기 상태.   
간단하고, 안정적, (짧은/긴 임계영역 모두) 널리 사용.   
짧은 임계구역에서 매우 자주 lock/unlock시, OS 커널 진입-이탈이 오버헤드가 될 수 있음 (하지만 실제 구현은 “하이브리드(짧은 스핀 + 블록)”를 활용해 최적화).   
   
   
### 2. std::shared_mutex   
std::shared_mutex는 Reader-Writer Lock(또는 RW Lock)의 C++ 표준화된 구현체 중 하나.   
동시에 여러 스레드가 '읽기' 접근만 하는 경우에는 공유 잠금(shared lock)을 통해 서로 병행 접근을 허용.   
단, 데이터 갱신('쓰기') 시에는 단독 잠금(exclusive lock)을 획득해야 하며, 이 기간에는 다른 스레드가 읽기라도 접근할 수 없다.   
읽기 위주(Write가 상대적으로 적음) 시나리오에서 성능 향상됨.   
일반 뮤텍스(std::mutex)는 여러 스레드가 동시에 읽기만 할 때도 하나의 스레드만 접근 가능.   
std::shared_mutex는 읽기만 하는 스레드 여러 개가 동시에 접근 가능 → 락 충돌(경합) 최소화.   
   
리더(Reader) 카운터와 라이터(Writer) 플래그를 관리.   
여러 개의 Reader가 “공유 락(shared lock)”을 획득하면, 내부적으로 카운터를 증가시킵니다.   
Writer가 들어오려면 해당 카운터가 0이 될 때까지 대기해야 하고, 단독 락(exclusive lock)을 설정한 후에 임계 영역에 들어갑니다.   
공유 잠금(shared_lock) 획득
   
내부적으로 “현재 Writer가 소유 중이 아닌지” 및 “Writer 대기 중인지” 등을 확인.   
만약 Writer가 없으면, readers_count를 1 증가시키고 바로 리턴(비교적 빠름).   
Writer가 대기 상태인 경우, 구현체에 따라 Reader가 선점해서 계속 들어갈 수도 있고, 새 Reader 진입을 잠글 수도 있음(공정성 정책에 따라 다름).   
단독 잠금(unique_lock) 획득.   
기존에 들어와 있는 Reader들이 다 빠져나가(readers_count == 0)야만 락 획득 가능.   
대기 중에는 OS 차원의 대기(커널 오브젝트) 또는 사용자 모드 스핀 등을 혼합하여 구현.   
공유 락 해제. readers_count를 1 감소.   
0이 되면 Writer가 대기 중이라면 깨워줄 수 있음.   
단독 락 해제 Writer 플래그를 해제.   
대기 중인 Reader나 Writer를 깨워줄 수 있음.   
   
   
### 3. std::recursive_mutex   
std::recursive_mutex는 C++ 표준에서 제공하는 뮤텍스의 한 종류로, 동일 스레드가 동일 뮤텍스를 여러 번 획득(중첩)할 수 있는 재귀 락 기능을 제공.   
보통의 std::mutex는 한 스레드가 lock()을 가진 상태에서 다시 같은 뮤텍스를 lock()하면 데드락이 발생하지만,   
std::recursive_mutex는 내부 카운터를 두어 스레드가 여러 번 lock()을 호출해도 뮤텍스를 이미 획득한 스레드이면 카운트를 증가시키고,   
unlock() 시 카운트를 감소시켜 0이 되면 실제로 뮤텍스를 해제.   
std::recursive_mutex는 내부에 소유 스레드 ID와 락 횟수(count)를 저장.   
lock()이 처음 호출되면 스레드 ID가 기록되고 count=1.   
동일 스레드가 다시 lock()을 호출하면 count++.   
unlock()할 때 count--. count==0이 되면 실제 뮤텍스를 풀어서 다른 스레드가 lock()할 수 있음.   

```cpp
#include <iostream>
#include <thread>
#include <mutex>

std::recursive_mutex rmtx;
int resourceCount = 0;

void funcA(int depth) {
    rmtx.lock(); // 재귀적으로 같은 뮤텍스
    std::cout << "funcA lock depth=" << depth << "\n";
    resourceCount++;
    if(depth > 0) {
        funcA(depth-1);
    }
    rmtx.unlock();
}

int main()
{
    std::thread t1([]{
        funcA(3); // 4번 lock 호출
    });
    std::thread t2([]{
        funcA(2); // 3번 lock 호출
    });
    t1.join(); 
    t2.join();

    std::cout << "resourceCount=" << resourceCount << "\n";
    return 0;
}
```
### 4. Critical Section   
User-mode lock. 충돌이 없는 경우(또는 매우 짧은 임계영역)에는 커널 오브젝트에 진입하지 않고 유저 모드에서 소유권을 획득할 수 있음.   
내부적으로 짧게 spin 후 실패 시, OS로 넘어가 WaitForSingleObject로 대기.   
커널 오브젝트(세마포어) 하나를 내부에서 필요 시만 동적 생성해 사용.   
프로세스 내(local)에서만 유효. 즉, 다른 프로세스와 공유 불가능.   
경량 오브젝트라 성능이 높음(짧은 임계영역 많이 잡을 때 우수).   
로컬 스레드 간 동기화에 매우 흔히 사용.   
프로세스 간 공유 불가.   
기능이 제한적(상태를 커스텀 시그널링할 수 없음, 스핀 횟수 제한 등만 세팅)   
```cpp
CRITICAL_SECTION cs;
InitializeCriticalSectionAndSpinCount(&cs, 2000); // spin count=2000
...
EnterCriticalSection(&cs); // acquire
// 임계영역
LeaveCriticalSection(&cs); // release
...
DeleteCriticalSection(&cs);
```
   
### 4. 커널 Mutex   
Windows에서 CreateMutex, WaitForSingleObject(mutex), ReleaseMutex로 사용하는 커널 레벨 Mutex.   
프로세스 간 공유 가능(이름 붙여서 여러 프로세스가 동일 Mutex 사용).   
한 스레드가 WaitForSingleObject로 획득, ReleaseMutex로 해제.   
항상 커널 모드 진입 → CRITICAL_SECTION보다 무거움.   
제공 기능: “Abandoned mutex” 감지(소유 스레드가 terminate되었을 때).   
크로스 프로세스/세션 동기화 가능.   
abandoned 감지.   
“lightweight”가 아님. 임계영역이 짧고 빈번히 lock/unlock 시엔 비효율(커널 전환).   
```cpp
HANDLE hMut = CreateMutex(NULL, FALSE, NULL); 
WaitForSingleObject(hMut, INFINITE);
// 임계영역
ReleaseMutex(hMut);
CloseHandle(hMut);
```
   
### 4. 세마포어   
Windows CreateSemaphore, WaitForSingleObject, ReleaseSemaphore.   
“카운팅 리소스” 개념. 최대 N개의 스레드가 동시에 통과 가능.   
0이면 대기, ReleaseSemaphore(n) 하면 카운터+n.   
커널 오브젝트(커널 모드 전환).   
한번에 “N개” 공유 가능.   
프로세스 간 공유 가능(이름 붙이거나 handle 복사).   
뮤텍스(1개 통과)보단 범용적이지만, lock/unlock(1개 통과) 용도로 쓰기는 좀 불편.   
커널 진입 오버헤드.   
   
```cpp
#include "stdafx.h"
#include <windows.h>
#include <stdio.h>

#define MAX_SEM_COUNT 3
#define THREADCOUNT 5

HANDLE ghSemaphore;

DWORD WINAPI ThreadProc( LPVOID );


int main(int argc, char* argv[])
{
 HANDLE aThread[THREADCOUNT]; // THREADCOUNT 12
    DWORD ThreadID[THREADCOUNT]; // THREADCOUNT 12
    int i;
 
    // Create a semaphore with initial and max counts of MAX_SEM_COUNT
    ghSemaphore = CreateSemaphore(
        NULL,           // default security attributes
        MAX_SEM_COUNT,  // initial count. MAX_SEM_COUNT 10
        MAX_SEM_COUNT,  // maximum count. MAX_SEM_COUNT 10
        NULL);          // unnamed semaphore
 
    if (ghSemaphore == NULL) // 세마포어가 생성되지 않았을때 에러 처리
    {
        printf("CreateSemaphore error: %d\n", GetLastError());
        return 1;
    }
 
    // Create worker threads
    for( i=0; i < THREADCOUNT; i++ ) // THREADCOUNT 12
    {
        // Thread를 생성한다
        aThread[i] = CreateThread(
                                   NULL,       // default security attributes
                                   0,          // default stack size
                                   (LPTHREAD_START_ROUTINE) ThreadProc,
                                   NULL,       // no thread function arguments
                                   0,          // default creation flags
                                   &ThreadID[i]); // receive thread identifier
  
        // Thread가 생성이 안되었을때 에러처리
        if( aThread[i] == NULL )
        {
            printf("CreateThread error: %d\n", GetLastError());
            return 1;
        }
        else
        {
            printf("Created Thread : %d \n", ThreadID[i]);
        }
    }
 
    // Wait for all threads to terminate 
    WaitForMultipleObjects(THREADCOUNT, aThread, TRUE, INFINITE);
/*  DWORD WINAPI WaitForMultipleObjects(          // 개체의 종료 이벤트를 대기 하는 함수
                   _In_  DWORD nCount,            // 기다릴 이벤트의 수
                   _In_  const HANDLE *lpHandles, // 기다릴 오브젝트들
                   _In_  BOOL bWaitAll,           // 모두 기다리는지(TRUE) 그중 하나만 기다리는지(FALSE)
                   _In_  DWORD dwMilliseconds  // 이벤트가 오기까지 기다리는 시간(단위:milliseconds) INFINITE:무한대
 ); */
 
    // Close thread and semaphore handles
    for( i=0; i < THREADCOUNT; i++ )
        CloseHandle(aThread[i]);
    CloseHandle(ghSemaphore);
    return 0;

} 
```
   
### 4. Event   
Windows에서 제공하는 커널 동기화 오브젝트 중 하나.   
CreateEvent, SetEvent, ResetEvent, WaitForSingleObject 등의 함수로 제어.   
보통 조건 변수나 수동/자동 리셋 이벤트를 구현하는 데 사용.   
이벤트가 신호 상태(signaled)면 WaitForSingleObject가 즉시 통과.   
이벤트가 “비신호 상태(non-signaled)”면 스레드를 블록 (OS 레벨).   
윈도우 전용이지만, 세부 제어가 가능(수동/자동 리셋), 여러 스레드가 한 오브젝트를 기다릴 수 있음.   
뮤텍스나 세마포어도 내부적으로 유사한 “WaitForSingleObject” 기반.   
커널 오브젝트라서 커널 모드 전환이 발생 → 짧은 임계영역에 남발하면 성능 떨어짐.   
C++ 표준 x, Windows API 의존적.   

```cpp
#include <windows.h>
#include <iostream>
#include <thread>
#include <vector>
#include <atomic>

std::atomic<int> sharedData2(0);

// 수동 리셋 이벤트
HANDLE gEvent = NULL;
std::atomic<bool> gLock(false); // spin-like flag

void lock_event()
{
    // 간단한 spin/cas
    bool expected = false;
    while(!gLock.compare_exchange_strong(expected, true)) {
        expected = false;
        // 실패 시 Wait
        WaitForSingleObject(gEvent, INFINITE);
        // 깨어나서 다시 시도
    }
}

void unlock_event()
{
    // unlock
    gLock.store(false);
    // SetEvent => 대기 중 스레드 하나 깨움
    SetEvent(gEvent);
}

void worker_event(int id, int iterations) {
    for(int i=0;i<iterations;i++){
        lock_event();
        sharedData2++;
        unlock_event();
    }
    std::cout << "Thread " << id << " done.\n";
}

int main()
{
    // CreateEvent(SECURITY, bManualReset=TRUE, initialState=TRUE, name=NULL)
    // 여기서는 AutoReset=FALSE => manual reset event
    gEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
    // FALSE, FALSE => auto-reset event (하나만 깨울 수 있음)
    if(!gEvent){
        std::cerr << "CreateEvent fail\n";
        return 1;
    }

    std::vector<std::thread> threads;
    for(int i=0; i<4; i++){
        threads.emplace_back(worker_event, i, 1000000);
    }
    for(auto & t : threads){
        t.join();
    }

    std::cout << "Final sharedData2=" << sharedData2.load() << "\n";
    CloseHandle(gEvent);
    return 0;
}
```

### 4. SpinLock   
CAS(Compare And Swap) 나 원자적 플래그를 사용해 락을 구현/   
락이 이미 누군가에게 획득되었다면, 스레드는 계속 루프를 돌며(spin) 락을 시도.   
짧은 임계영역에서 충돌이 적은 경우 매우 빠를 수 있음 (OS 커널 진입 X).   
context switch 없이 락을 얻을 때까지 CPU를 소모.   
충돌이 심하면 CPU를 대거 낭비 (idle waiting보다 비효율).   
짧은 임계영역이 아니라면 spinlock은 성능이 나빠질 수 있음.   

```cpp
#include <atomic>
#include <iostream>
#include <thread>
#include <vector>

class SpinLock {
public:
    SpinLock() : flag(false) {}

    void lock() {
        bool expected = false;
        while(!flag.compare_exchange_weak(expected, true)) {
            expected = false;
            // 바쁜 대기 (spin)
            // 짧게 _mm_pause() or std::this_thread::yield() 가능
        }
    }

    void unlock() {
        flag.store(false);
    }

private:
    std::atomic<bool> flag;
};

int sharedData3 = 0;
SpinLock sLock;

void worker_spin(int id, int iterations) {
    for(int i=0; i<iterations; i++){
        sLock.lock();
        sharedData3++;
        sLock.unlock();
    }
    std::cout << "Thread " << id << " done.\n";
}

int main()
{
    std::vector<std::thread> threads;
    for(int i=0; i<4; i++){
        threads.emplace_back(worker_spin, i, 1000000);
    }
    for(auto & t : threads){
        t.join();
    }
    std::cout << "Final sharedData3=" << sharedData3 << "\n";
    return 0;
}
```   

    
