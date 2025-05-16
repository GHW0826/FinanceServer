
#include <iostream>
#include <vector>
#include <thread>
#include <chrono>
#include <atomic>

/*
using namespace std;
using namespace std::chrono;

static const int NUM_OPS_SINGLE = 10'000'000;
static const int NUM_OPS_MULTI = 5'000'000;

#pragma once

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

// 단일 스레드 벤치마크
long long BenchmarkSingleThread(size_t blockSize, int opsCount)
{
    MemoryPool pool(blockSize);
    
    auto start = high_resolution_clock::now();
    for (int i = 0; i < opsCount; i++) {
        void* p = pool.Pop();
        // (옵션) p 사용
        // pool.Push(p);
    }
    auto end = high_resolution_clock::now();

    long long durationNs = duration_cast<nanoseconds>(end - start).count();
    return durationNs;
}

// 멀티스레드 벤치마크
long long BenchmarkMultiThread(size_t blockSize, int threadCount, int opsPerThread)
{
    MemoryPool pool(blockSize);

    auto start = high_resolution_clock::now();

    vector<thread> threads;
    threads.reserve(threadCount);

    for (int t = 0; t < threadCount; t++) {
        threads.emplace_back([&pool, opsPerThread]() {
            for (int i = 0; i < opsPerThread; i++) {
                void* p = pool.Pop();
                // (옵션) p 사용
                // pool.Push(p);
            }
        });
    }
    for (auto& th : threads) {
        th.join();
    }

    auto end = high_resolution_clock::now();
    long long durationNs = duration_cast<nanoseconds>(end - start).count();
    return durationNs;
}

int main()
{
    cout << "=== TestCase A: Single Thread, Block Size = 128, 10M ops ===\n";
    {
        size_t blockSize = 128;
        int ops = NUM_OPS_SINGLE;
        long long ns = BenchmarkSingleThread(blockSize, ops);
        double nsPerOp = (double)ns / ops;
        cout << "Time: " << ns << " ns, ns/ops: " << nsPerOp << endl;
    }

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
        vector<size_t> blockSizes = {32, 128, 512, 1024};
        for (auto bs : blockSizes) {
            cout << "Block Size: " << bs << "\n";

            // Single
            {
                long long ns = BenchmarkSingleThread(bs, NUM_OPS_SINGLE);
                double nsOp = (double)ns / NUM_OPS_SINGLE;
                cout << "   Single Thread => " << ns << " ns total, " << nsOp << " ns/ops\n";
            }
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
