#pragma once
#include <iostream>
#include <iomanip>
#include <atomic>
#include <vector>
#include <mutex>
#include <random>
#include "CSLock.h"
#include "EventLock.h"
#include "SYLock.h"
#include "SpinLock.h"
#include "SSharedLock.h"
#include "StdLock.h"
#include "SELock.h"
#include "MutexLock.h"
// #include "SYSharedLock.h"
#include "StdSharedLock.h"
#include "StdRecursiveLock.h"
#include "ITestLockGuard.h"

using namespace std;

extern int sharedData;
extern int LongCSms;

template<typename T>
void ShortCriticalSection(T& lock, int threadOps) 
{
	for (int i = 0; i < threadOps; ++i) {
		WriteLockGuard wlg(lock);
		sharedData++;
	}
}

template<typename T>
void LongCriticalSection(T& lock, int threadOps) 
{
	for (int i = 0; i < threadOps; ++i) {
		WriteLockGuard wlg(lock);
		std::this_thread::sleep_for(std::chrono::microseconds(LongCSms));
	}
}

template<typename T>
void RWScenario(T& rwLock, int threadOps, std::vector<bool>& read)
{
    for (int i = 0; i < threadOps; ++i) {
        ReadLockGuard<T> rlg(rwLock);
        int val = sharedData;
        (void)val;
    }
}

template<typename T>
void BenchmarkShort(T& lockImpl, const char* lockName, int threadCount, int opsPerThread) {
	sharedData = 0;
	auto start = std::chrono::high_resolution_clock::now();

	std::vector<std::thread> threads;
	for (int i = 0; i < threadCount; ++i)
		threads.emplace_back(std::thread(&ShortCriticalSection<T>, std::ref(lockImpl), opsPerThread));
	for (auto& th : threads)
		th.join();

	auto end = std::chrono::high_resolution_clock::now();
	auto dur = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();

    cout << left << setw(12) << "[ShortCS]"
        << " Lock=" << setw(25) << lockName
        << " time=" << right << setw(10) << dur / 1000 << "ms" << left
        << " Threads=" << setw(4) << threadCount
        << " OpsPerThread=" << setw(8) << opsPerThread
   //     << " => total=" << setw(10) << (threadCount * opsPerThread)
        << " sharedData=" << setw(10) << sharedData << endl;
}


template<typename T>
void BenchmarkLong(T& lockImpl, const char* lockName, int threadCount, int opsPerThread) {
    sharedData = 0;
    auto start = std::chrono::high_resolution_clock::now();

    std::vector<std::thread> threads;
    for (int i = 0; i < threadCount; ++i)
        threads.emplace_back(std::thread(&LongCriticalSection<T>, std::ref(lockImpl), opsPerThread));
    for (auto& th : threads)
        th.join();

    auto end = std::chrono::high_resolution_clock::now();
    auto dur = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();

    cout << left << setw(20) << "[LongCS]"
        << " Lock=" << setw(25) << lockName
        << " time=" << right << setw(10) << (dur / 1000) << "ms" << left   // fs < ps < ns < us < ms < s < m < h
        << " Threads=" << setw(4) << threadCount
        << " OpsPerThread=" << setw(8) << opsPerThread
      //  << " => total=" << setw(10) << (threadCount * opsPerThread)
        << " sharedData=" << setw(10) << sharedData << endl;
}

template<typename T>
void BenchmarkRW(T& rwLock, const char* lockName, int threadCount, int opsPerThread, double readRatio)
{
    int readOps = opsPerThread * readRatio;
    std::vector<bool> read(opsPerThread);
    for (int i = 0; i < read.size(); ++i) {
        if (i < readOps) {
            read[i] = true;
        }
        else {
            read[i] = false;
        }
    }
    auto rng = std::default_random_engine{};
    std::shuffle(read.begin(), read.end(), rng);

    sharedData = 0;
    auto start = std::chrono::high_resolution_clock::now();

    std::vector<std::thread> threads;
    for (int i = 0; i < threadCount; ++i)
        threads.emplace_back(RWScenario<T>, std::ref(rwLock), opsPerThread, std::ref(read));
    for (auto& th : threads)
        th.join();

    auto end = std::chrono::high_resolution_clock::now();
    auto dur = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
    // CPU 사용률(%) ~ ( totalCpuMs / wallMs ) * 100
    cout << left << setw(8) << "[RWCS]" 
        << " Lock=" << setw(25) << lockName
        << " t=" << right << setw(10) << dur / 1'000 << "ms" << left
        << " wr%=" << (1 - readRatio)
        << " rr%=" << readRatio
        << " thrs=" << setw(2) << threadCount
        << " OpsPthr=" << setw(8) << opsPerThread
        << std::fixed << std::setprecision(0) << " WOpsPthr=" << setw(8) << (opsPerThread * (1 - readRatio))
        << std::fixed << std::setprecision(0) << " ROpsPthr=" << setw(8) << (opsPerThread * readRatio)
      //  << " => total=" << setw(8) << (threadCount * opsPerThread)
        << " data=" << setw(8) << sharedData << endl;
}


template<typename T>
void RunShortBenchmarks(const vector<pair<const char*, T*>>& locks, int threadCnt, int opsCnt) {
    for (const auto& [name, lock] : locks) {
        BenchmarkShort(*lock, name, threadCnt, opsCnt);
    }
}

template<typename T>
void RunLongBenchmarks(const vector<pair<const char*, T*>>& locks, int threadCnt, int opsCnt) {
    for (const auto& [name, lock] : locks) {
        BenchmarkLong(*lock, name, threadCnt, opsCnt);
    }
}

template<typename T>
void RunRWBenchmarks(const vector<pair<const char*, T*>>& locks, int threadCnt, int opsCnt, double readRatio) {
    for (const auto& [name, lock] : locks) {
        BenchmarkRW(*lock, name, threadCnt, opsCnt, readRatio);
    }
}

