#include <iostream>
#include "MemoryPoolTest.h"

void PoolBenchmarkTest(vector<pair<const char*, SMemoryBase*>>& testArr)
{
    int sumOpsCnt = 1'000'000;
    int threadCnt = 1;
    int opsCnt = sumOpsCnt / threadCnt; 
    int minAllocSize = 5'000'000;
    int maxAllocSize = 6'000'000;
    const auto cpuCnt = std::thread::hardware_concurrency();

    std::random_device rd;
    std::mt19937 rng(rd());
    std::uniform_int_distribution<int> dist(minAllocSize, maxAllocSize);
    int randomAllocCnt = dist(rng);

    std::vector<int> opsOrder(randomAllocCnt, 1);
    for (int i = 0; i < randomAllocCnt; ++i) {
        opsOrder.push_back(0);
    }
    std::shuffle(opsOrder.begin(), opsOrder.end(), rng);

    RunPoolBenchmarks<SMemoryBase>(testArr, opsOrder, threadCnt, opsCnt);
    cout << "\n";
}


int main()
{
    {
        NoPoolMemory npm;
        vector<pair<const char*, SMemoryBase*>> testArr = {
            {"NoPoolMemory", &npm },
        };
        PoolBenchmarkTest(testArr);
    }
    // visual studio 메모리 ui 딜레이때문에 넣음
    this_thread::sleep_for(5s);
    {
        LockFreeStackMemory lfsm;
        vector<pair<const char*, SMemoryBase*>> testArr = {
            {"LockFreeStackMemory", &lfsm },
        };
        PoolBenchmarkTest(testArr);
    }
    this_thread::sleep_for(5s);

    return 0;
}
