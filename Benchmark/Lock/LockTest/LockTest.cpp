#include <iostream>
#include "test.h"

int LongCSms = 100;
int sharedData = 0;
int OpsCnt = 1'000'000;

void ShortBenchmarkTest(const vector<pair<const char*, ILockTest*>>& testArr)
{
	int sumOpsCnt = OpsCnt;
	int threadCnt = 1;
	int opsCnt = sumOpsCnt / threadCnt;
	vector<int> threadCnts = { 1, 4, 8, 16, 25 };
	const auto cpuCnt = std::thread::hardware_concurrency();

	for (int i = 0; i < threadCnts.size(); ++i) {
		threadCnt = threadCnts[i];
		opsCnt = sumOpsCnt / threadCnt;
		RunShortBenchmarks<ILockTest>(testArr, threadCnt, opsCnt);
		cout << "\n";
	}
}

void LongBenchmarkTest(const vector<pair<const char*, ILockTest*>>& testArr)
{
	int sumOpsCnt = 1'00;// OpsCnt;
	int threadCnt = 1;
	int opsCnt = sumOpsCnt / threadCnt;
	vector<int> threadCnts = { 1, 4, 8, 16, 25 };
	const auto cpuCnt = std::thread::hardware_concurrency();

	for (int i = 0; i < threadCnts.size(); ++i) {
		threadCnt = threadCnts[i];
		opsCnt = sumOpsCnt / threadCnt;
		RunLongBenchmarks<ILockTest>(testArr, threadCnt, opsCnt);
		cout << "\n";
	}
}

void RWBenchmarkTest(const vector<pair<const char*, ILockTest*>>& testArr)
{
	int sumOpsCnt = OpsCnt;
	int threadCnt = 1;
	int opsCnt = sumOpsCnt / threadCnt;
	vector<int> threadCnts = { 1, 4, 8, 16, 25 };
	vector<double> readRatios = { 0.0, 0.1, 0.25, 0.5, 0.75, 1.0 };
	double readRatio = 1.0;
	const auto cpuCnt = std::thread::hardware_concurrency();

	for (int i = 0; i < threadCnts.size(); ++i) {
		threadCnt = threadCnts[i];
		opsCnt = sumOpsCnt / threadCnt;
		RunRWBenchmarks<ILockTest>(testArr, threadCnt, opsCnt, readRatio);
		cout << "\n";
	}
}

int main()
{
	StdLock stdl;
	StdSharedLock stdsl;
	StdRecursiveLock stdrl;
	CriticalSectionLock cs1(500);
	CriticalSectionLock cs2(1000);
	CriticalSectionLock cs3(2000);
	CriticalSectionLock cs4(5000);
	CriticalSectionLock cs5(10000);
	EventLock evl;
	MutexLock ml;
	SSharedLock ssl;
	SpinLock sl;
	SYLock syl1(500);
	SYLock syl2(1000);
	SYLock syl3(2000);
	SYLock syl4(5000);
	SYLock syl5(10000);
	SELock sel1(500);
	SELock sel2(1000);
	SELock sel3(2000);
	SELock sel4(5000);
	SELock sel5(10000);
	vector<pair<const char*, ILockTest*>> testArr = {
		{"StdLock", &stdl},
		{"StdSharedLock", &stdsl},
		{"StdRecursiveLock", &stdrl},
		{"CriticalSection(500)", &cs1},
		{"CriticalSection(1000)", &cs2},
		{"CriticalSection(2000)", &cs3},
		{"CriticalSection(5000)", &cs4},
		{"CriticalSection(10000)", &cs5},
		{"EventLock", &evl},
		{"MutexLock", &ml},
		{"SSharedLock", &ssl},
		{"SpinLock", &sl},
		{"SYLock(500)", &syl1},
		{"SYLock(1000)", &syl2},
		{"SYLock(2000)", &syl3},
		{"SYLock(5000)", &syl4},
		{"SYLock(10000)", &syl5},
		{"SELock(500)", &sel1},
		{"SELock(1000)", &sel2},
		{"SELock(2000)", &sel3},
		{"SELock(5000)", &sel4},
		{"SELock(10000)", &sel5}
	};

	// ShortBenchmarkTest(testArr);
	LongBenchmarkTest(testArr);
	// RWBenchmarkTest(testArr);
}
