#pragma once
#include <iostream>
#include <thread>
#include <vector>
#include <queue>
#include <functional>
#include <mutex>
#include <condition_variable>
#include <chrono>
#include "ThreadManager.h"

// -----------------------------------------------------------------
// 예시 작업 함수: CPU 바운드
// (큰 반복문)
std::mutex _gm;
unsigned long long sum = 0;
void cpuBoundTask(int id)
{
    std::lock_guard<std::mutex> lg(_gm);
    for (long i = 0; i < 2400000000000000000; ++i) {
        ++sum;
    }
}

// 예시 작업 함수: I/O 바운드 흉내
// (10ms sleep)
void ioBoundTask(int id)
{
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
}

// -----------------------------------------------------------------
// (1) ThreadPool 미사용(naive): 작업마다 새 스레드 생성
// -----------------------------------------------------------------
void runWithoutThreadPool(int taskCount, bool isCPU) {
    auto start = std::chrono::high_resolution_clock::now();

    std::vector<std::thread> threads;
    threads.reserve(taskCount);

    for (int i = 0; i < taskCount; ++i) {
        // 작업마다 새 스레드 생성
        if (isCPU) {
            threads.emplace_back(cpuBoundTask, i);
        }
        else {
            threads.emplace_back(ioBoundTask, i);
        }
    }

    // join
    for (auto& t : threads) {
        t.join();
    }

    auto end = std::chrono::high_resolution_clock::now();
    auto dur = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

    std::cout << "[NoThreadPool] Tasks=" << taskCount
        << ", sum= " << sum
        << (isCPU ? " [CPU]" : " [IO]") << " => Time=" << dur << " ms\n";
}

// -----------------------------------------------------------------
// (2) ThreadManager (스레드 풀 유사 구조) 사용
// -----------------------------------------------------------------
void runWithThreadManager(int taskCount, bool isCPU, int threadCount) {
    // ThreadManager 생성
    ThreadManager manager(threadCount);

    auto start = std::chrono::high_resolution_clock::now();

    // 작업 enqueue
    for (int i = 0; i < taskCount; ++i) {
        if (isCPU) {
            manager.enqueue([i]() { cpuBoundTask(i); });
        }
        else {
            manager.enqueue([i]() { ioBoundTask(i); });
        }
    }

    // ThreadManager는 소멸자에서 join을 수행하므로
    // 여기서는 약간 대기하거나, 작업 완료를 기다리는 추가 로직이 필요
    // 여기서는 간단히 2~3초 대기 (테스트용)
    if (isCPU) {
        std::this_thread::sleep_for(std::chrono::seconds(3));
    }
    else {
        // IO 바운드는 작업 하나당 10ms면,
        // 대략 (taskCount * 10ms / threadCount) 정도 예상
        // 여기서는 간단히 3초
        std::this_thread::sleep_for(std::chrono::seconds(3));
    }

    auto end = std::chrono::high_resolution_clock::now();
    auto dur = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

    std::cout << "[ThreadManager] Threads=" << threadCount
        << ", sum= " << sum 
        << ", Tasks=" << taskCount << (isCPU ? " [CPU]" : " [IO]") << " => Time=" << dur << " ms\n";
}
