#pragma once
#include <chrono>
#include <iostream>
#include <vector>
#include <thread>
#include <atomic>
#include <random>
#include <unordered_map>
#include "ITestMemoryPool.h"
#include "SMemory.h"
#include "NoPoolMemory.h"
#include "LockFreeStackMemory.h"

using namespace std;
using namespace std::chrono;

class TestObject 
{
public:
    TestObject()
    {
    }
    TestObject(int a, int b) 
    {
    }
public:
    long long x[1000];
    long long y[1000];
};

template< typename AllocType, typename Type, typename... Args>
Type* snew(AllocType& allocType, Args&&... args)
{
    Type* memory = static_cast<Type*>(allocType.Allocate(sizeof(Type)));
    new(memory)Type(forward<Args>(args)...); // placement new
    return memory;
}

template<typename AllocType, typename Type>
void sdelete(AllocType& allocType, Type* ptr)
{
    ptr->~Type();
    allocType.Release(ptr);
}


template<typename T>
void MemoryLogic(std::vector<TestObject*>& ptrs, std::vector<int>& opsOrder, T& pool, int threadOps)
{
    unordered_map<int, TestObject*> table;
    int insertIndex = 0;
    int deleteIndex = 0;
    for (int i = 0; i < opsOrder.size(); i++) {
        // alloc
        if (opsOrder[i] == 1) {
            TestObject* obj = snew<T, TestObject>(pool);
            table.insert({ insertIndex, obj });
            ++insertIndex;
        }
        else {
            if (table.size() > 0) {
                // release
                sdelete<T, TestObject>(pool, table[deleteIndex]);
                table[deleteIndex] = nullptr;
                table.erase(deleteIndex);
                ++deleteIndex;
            }
        }
    }

    for (auto p : table) {
        if (p.second != nullptr) {
            sdelete<T, TestObject>(pool, p.second);
            table[deleteIndex] = nullptr;
        }
    }
}

template<typename T>
void PoolBenchmark(T& pool, const char* poolName, std::vector<int>& opsOrder, int threadCount, int opsPerThread)
{
    std::vector<TestObject*> ptrs;
    ptrs.reserve(opsOrder.size());

    vector<thread> threads;
    threads.reserve(threadCount);
    
    auto start = std::chrono::high_resolution_clock::now();

    for (int t = 0; t < threadCount; t++) {
        threads.emplace_back(std::thread(&MemoryLogic<T>, std::ref(ptrs), std::ref(opsOrder), std::ref(pool), opsPerThread));
        /*
        threads.emplace_back([&pool, opsPerThread]() {
            for (int i = 0; i < opsPerThread; i++) {
                void* p = pool.Pop();
                // (옵션) p 사용
                // pool.Push(p);
            }
        });
        */
    }
    for (auto& th : threads) {
        th.join();
    }

    auto end = std::chrono::high_resolution_clock::now();
    auto dur = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

    cout << left << setw(12) << "[MemoryPool]"
        << " Pool=" << setw(50) << poolName
        << " time=" << right << setw(10) << dur << "us" << left
        << " Threads=" << setw(4) << threadCount
        << " OpsPerThread=" << setw(8) << opsPerThread
        << " => total=" << setw(10) << (threadCount * opsPerThread) << endl;
}

template<typename T>
void RunPoolBenchmarks(const vector<pair<const char*, T*>>& locks, std::vector<int>& opsOrder, int threadCnt, int opsCnt) {
    for (const auto& [name, lock] : locks) {
        PoolBenchmark(*lock, name, opsOrder, threadCnt, opsCnt);
    }
}

/*
int test()
{
    //cout << "=== TestCase A: Single Thread, Block Size = 128, 10M ops ===\n";
    //{
    //    size_t blockSize = 128;
    //    int ops = NUM_OPS_SINGLE;
    //    long long ns = BenchmarkSingleThread(blockSize, ops);
    //    double nsPerOp = (double)ns / ops;
    //    cout << "Time: " << ns << " ns, ns/ops: " << nsPerOp << endl;
    //}

    cout << "\n=== TestCase B: 4 Threads, Block Size = 128, 5M ops each ===\n";
    {
        size_t blockSize = 128;
        int threadCount = 4;
        int opsPerThread = NUM_OPS_MULTI;
        long long ns = BenchmarkMultiThread(blockSize, threadCount, opsPerThread);
        long long totalOps = (long long)threadCount * opsPerThread;
        double nsPerOp = (double)ns / totalOps;
        cout << "Time: " << ns << " ns, ns/ops: " << nsPerOp << endl;
    }

    cout << "\n=== TestCase C: Varying block size (32, 128, 512, 1024) Single + Multi\n";
    {
        vector<size_t> blockSizes = { 32, 128, 512, 1024 };
        for (auto bs : blockSizes) {
            cout << "Block Size: " << bs << "\n";

            //// Single
            //{
            //    long long ns = BenchmarkSingleThread(bs, NUM_OPS_SINGLE);
            //    double nsOp = (double)ns / NUM_OPS_SINGLE;
            //    cout << "   Single Thread => " << ns << " ns total, " << nsOp << " ns/ops\n";
            //}
            // Multi (4 threads)
            {
                int threadCount = 4;
                long long ns = BenchmarkMultiThread(bs, threadCount, NUM_OPS_MULTI);
                long long totalOps = (long long)threadCount * NUM_OPS_MULTI;
                double nsOp = (double)ns / totalOps;
                cout << "   4 Threads     => " << ns << " ns total, " << nsOp << " ns/ops\n";
            }
        }
    }

    cout << "\n=== TestCase D: Large scale => 16 threads, block size = 256, 100M total ops ===\n";
    {
        size_t blockSize = 256;
        int threadCount = 16;
        int opsPerThread = 6'250'000; // total ~100M ops
        long long ns = BenchmarkMultiThread(blockSize, threadCount, opsPerThread);
        long long totalOps = (long long)threadCount * opsPerThread;
        double nsOp = (double)ns / totalOps;
        cout << "Time: " << ns << " ns, ns/ops: " << nsOp << endl;
    }

    return 0;
}
*/
