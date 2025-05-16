1. 벤치마크 시나리오
1.1. 다양한 작업 패턴
Short Critical Section
임계영역이 매우 짧고, 락 잡은 후 간단히 ++ 연산 등만 수행
스핀락이 유리할 수 있음

Long Critical Section
락을 잡고 수 ms~수십 ms 동안 처리 (예: 파일 I/O, 복잡 연산)
OS 뮤텍스(블로킹)나 RWLock이 유리할 수도

Mixed
임계영역 시간이 짧을 때도 있고 길 때도 있는 혼합
Read-Write 비율
여러 스레드가 공용 데이터 구조(예: balanced tree, map 등)에 읽기(80%) vs 쓰기(20%) 분포.
RWLock(shared_lock)에서 동시에 여러 스레드가 읽기 가능한 상황이 실제 성능에 어떻게 기여하는지 본다.


1.2. 스레드 개수(동시성)
1, 2, 4, 8, 16, … 등으로 늘려가며
lock 횟수(ops)나 throughput(초당 연산)을 측정
“스레드 증가에 따른 확장성(Scalability)”을 그래프로 그리면 각 락이 어느 구간에서 병목인지 잘 보인다.


1.3. 측정 지표
Throughput (ops/s)
초당 몇 번의 lock/unlock 연산을 처리할 수 있는지
예: 1억 번 increment 시도, 걸린 시간 => ops/s

Latency (ns/ops, 또는 평균 락 대기 시간)
한 번 lock/unlock 하는 데 걸리는 평균 시간

CPU 사용량
스핀락은 충돌이 많으면 CPU를 많이 쓰므로, CPU usage가 높아질 수 있음

충돌(경합) 시 성능
임계영역 점유가 길거나, 스레드 수가 많을 때 어떤 락이 오래 걸리는가

정확성
최종 결과 (예: sharedData += 1)를 1억 번 했으면, 최종값이 1억 × (스레드 수)가 맞는지.


3. 예제 코드 구조
하나의 통합 벤치마크 프로그램에서, 다양한 락을 “동일한 시나리오”로 테스트하도록 작성할 수 있습니다.

2.1. 공통 인터페이스
cpp
복사
편집
struct ILock {
    virtual ~ILock() {}
    virtual void lock() = 0;
    virtual void unlock() = 0;
};

// Read-Write lock용
struct IRWLock {
    virtual ~IRWLock() {}
    virtual void lock_shared() = 0;
    virtual void unlock_shared() = 0;
    virtual void lock_exclusive() = 0;
    virtual void unlock_exclusive() = 0;
};
각 구현(rokiss spinlock, GPT spinlock, std::mutex, etc.)을 ILock 또는 IRWLock에 맞게 래핑(wrapper)해주면, 동일한 테스트 코드를 재사용 가능.
2.2. 락 구현 Wrappers
(A) SpinLock (rokiss, gpt) 예시
cpp
복사
편집
class SpinLock_Rokiss : public ILock {
public:
    void lock() override {
        // rokiss spinlock lock
    }
    void unlock() override {
        // rokiss spinlock unlock
    }
};

class SpinLock_Gpt : public ILock {
public:
    void lock() override {
        // gpt spinlock lock
    }
    void unlock() override {
        // gpt spinlock unlock
    }
};
(B) std::mutex 래퍼
cpp
복사
편집
#include <mutex>

class StdMutexLock : public ILock {
public:
    void lock() override { mtx.lock(); }
    void unlock() override { mtx.unlock(); }
private:
    std::mutex mtx;
};
(C) SRWLOCK (Windows 전용)
cpp
복사
편집
#ifdef _WIN32
#include <Windows.h>
class SRWLockWrapper : public ILock {
public:
    SRWLockWrapper() { InitializeSRWLock(&lock_); }
    void lock() override { AcquireSRWLockExclusive(&lock_); }
    void unlock() override { ReleaseSRWLockExclusive(&lock_); }
private:
    SRWLOCK lock_;
};
#endif
(D) SharedLock (C++17) / Boost SharedLock
cpp
복사
편집
#include <shared_mutex> // for std::shared_mutex
// ...
class StdSharedMutex : public IRWLock {
public:
    void lock_shared() override {
        smtx.lock_shared();
    }
    void unlock_shared() override {
        smtx.unlock_shared();
    }
    void lock_exclusive() override {
        smtx.lock();
    }
    void unlock_exclusive() override {
        smtx.unlock();
    }
private:
    std::shared_mutex smtx;
};

// Boost shared_mutex 유사 (boost::shared_mutex)
(E) Recursive Mutex (std::recursive_mutex)
cpp
복사
편집
class StdRecursiveMutexLock : public ILock {
public:
    void lock() override { rmtx.lock(); }
    void unlock() override { rmtx.unlock(); }
private:
    std::recursive_mutex rmtx;
};
(F) Recursive SpinLock
cpp
복사
편집
class RecursiveSpinLock_Gpt : public ILock {
public:
    void lock() override { /* ... */ }
    void unlock() override { /* ... */ }
};
3. 실제 벤치마크 코드
3.1. 시나리오: short critical section
cpp
복사
편집
#include <atomic>
#include <chrono>
#include <iostream>
#include <vector>
#include <thread>

int sharedData = 0;

void shortCriticalSection(ILock& lock, int threadOps) {
    for (int i=0; i<threadOps; i++) {
        lock.lock();
        // very short: just increment
        sharedData++;
        lock.unlock();
    }
}

void BenchmarkShort(ILock& lockImpl, const char* lockName, int threadCount, int opsPerThread) {
    sharedData = 0;
    auto start = std::chrono::high_resolution_clock::now();

    std::vector<std::thread> threads;
    for (int i=0; i<threadCount; i++) {
        threads.emplace_back(shortCriticalSection, std::ref(lockImpl), opsPerThread);
    }
    for (auto& th : threads) th.join();

    auto end = std::chrono::high_resolution_clock::now();
    auto dur = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();

    std::cout << "[ShortCS] Lock=" << lockName 
              << " Threads=" << threadCount 
              << " OpsPerThread=" << opsPerThread 
              << " => total=" << (threadCount*opsPerThread)
              << " sharedData=" << sharedData
              << " time=" << dur << "us\n";
}
3.2. 시나리오: long critical section
cpp
복사
편집
void longCriticalSection(ILock& lock, int threadOps) {
    for (int i=0; i<threadOps; i++) {
        lock.lock();
        // simulate some "heavy" work
        std::this_thread::sleep_for(std::chrono::microseconds(50));
        lock.unlock();
    }
}
void BenchmarkLong(ILock& lockImpl, const char* lockName, int threadCount, int opsPerThread) {
    auto start = std::chrono::high_resolution_clock::now();

    std::vector<std::thread> threads;
    for (int i=0; i<threadCount; i++) {
        threads.emplace_back(longCriticalSection, std::ref(lockImpl), opsPerThread);
    }
    for (auto& th : threads) th.join();

    auto end = std::chrono::high_resolution_clock::now();
    auto dur = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

    std::cout << "[LongCS] Lock=" << lockName 
              << " Threads=" << threadCount 
              << " OpsPerThread=" << opsPerThread 
              << " time=" << dur << "ms\n";
}
3.3. 시나리오: Read-Write (IRWLock)
cpp
복사
편집
#include <random>

// 80% read, 20% write
void rwScenario(IRWLock& rwLock, int threadOps, double readRatio) {
    std::mt19937 rng(std::random_device{}());
    std::uniform_real_distribution<double> dist(0.0, 1.0);

    for (int i=0; i<threadOps; i++) {
        double r = dist(rng);
        if (r < readRatio) {
            rwLock.lock_shared();
            // read sharedData (just read)
            int val = sharedData;
            (void)val; // do something?
            rwLock.unlock_shared();
        } else {
            rwLock.lock_exclusive();
            // write
            sharedData++;
            rwLock.unlock_exclusive();
        }
    }
}

void BenchmarkRW(IRWLock& rwLock, const char* lockName, int threadCount, int opsPerThread, double readRatio) {
    sharedData = 0;
    auto start = std::chrono::high_resolution_clock::now();

    std::vector<std::thread> threads;
    for (int i=0; i<threadCount; i++) {
        threads.emplace_back(rwScenario, std::ref(rwLock), opsPerThread, readRatio);
    }
    for (auto& th : threads) th.join();

    auto end = std::chrono::high_resolution_clock::now();
    auto dur = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();

    std::cout << "[RW] Lock=" << lockName 
              << " Threads=" << threadCount 
              << " ReadRatio=" << readRatio
              << " => time=" << dur << "us\n";
}
4. 어떤 점을 눈여겨볼까?
Throughput vs. Latency

Throughput: 전체 ops를 수행하는 데 걸린 총 시간, 즉 “ops/s”나 “time per op”
Latency: 락 획득까지 대기 시간이 중요한 상황도 존재
스레드 수 증가

스레드를 1,2,4,8,16,... 늘렸을 때 성능이 어떻게 변하는가?
Spinlock은 충돌이 많아지면 성능 떨어질 수 있고, OS mutex는 context switch 비용이 커질 수도
CPU 사용량

Spinlock 계열은 충돌이 심하면 CPU usage가 높아짐
OS 커널 락(SRWLock, std::mutex) 계열은 스레드를 블록시켜 CPU 사용 줄일 수 있음
Read-Write Lock 효과

읽기 비중이 높은(80~90% Read) 시나리오에서 shared_lock(Boost, C++17)과 SRWLOCK(Shared Mode)의 이점이 큰지 확인
쓰기 비중이 높으면 그냥 mutex랑 별 차이 없을 수도
Recursive 특성

재귀 락(예: std::recursive_mutex, recursive spinlock)에서 “동일 스레드가 여러 번 lock()”할 때 성능 영향?
실제로 recursive가 필요한지, 단지 기능만 시험해볼지
안정성

잘못된 구현(특히 spinlock)에서 데드락이나 CRASH가 발생하지 않는지
5. 종합 Main
cpp
복사
편집
int main() {
    // 1) 준비: 여러 락 구현 객체 생성
    //     SpinLock_Rokiss rokissSpin;
    //     SpinLock_Gpt    gptSpin;
    //     StdMutexLock    stMutex;
    //     SRWLockWrapper  srw;
    //     ...
    //     StdRecursiveMutexLock recMutex;
    //     RecursiveSpinLock_Gpt recSpin;
    //     etc.

    // 2) shortCS benchmark
    //    BenchmarkShort(rokissSpin, "rokissSpin", 4, 100000);
    //    BenchmarkShort(gptSpin,   "gptSpin",   4, 100000);
    //    BenchmarkShort(stMutex,   "std::mutex",4, 100000);
    //    ...
    
    // 3) longCS benchmark
    //    BenchmarkLong(rokissSpin, "rokissSpin", 4, 1000);
    //    BenchmarkLong(stMutex,   "std::mutex", 4, 1000);
    //    ...
    
    // 4) RW scenario
    //   - For IRWLock interface only
    //   BenchmarkRW(stdShared, "std::shared_mutex", 4, 100000, 0.8); 
    //   ...
    
    return 0;
}
원하는 테스트 시나리오별로 함수를 호출하여 결과를 로그
결과를 표나 그래프로 정리


결론
벤치마크 시나리오:
(A) 짧은 임계영역 vs. (B) 긴 임계영역 vs. (C) RW 비율
스레드 수 (1,2,4,8,16...) 증가
성능(ops/s, time per op), CPU usage, 대기 시간
예제 코드:
공통 인터페이스(ILock, IRWLock) + 각 락 구현 Wrapper + 여러 시나리오 함수
shortCS: 단순 ++
longCS: sleep이나 긴 계산
RW scenario: 80% read, 20% write (등등)
눈여겨볼 점:
스레드 수가 많을 때 spinlock이 치명적일 수 있음
RWLock의 읽기 병행성 장점
recursive 락은 “동일 스레드”가 여러번 lock()시 성능/논리 검사
타임아웃, deadlock 여부, CPU usage
이런 구조로 각 락 구현(rokiss spinlock, GPT spinlock, std::mutex, std::shared_mutex, Boost shared_lock, SRWLock, recursive_mutex, recursive_spinlock)을 동일 시나리오에 비교 실험하면, 어떤 경우에 어떤 락이 더 효율적인지 한눈에 파악할 수 있을 것입니다.
