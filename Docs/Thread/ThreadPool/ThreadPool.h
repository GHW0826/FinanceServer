#pragma once

#include <thread>
#include <functional>

class ThreadPool
{
public:
	ThreadPool()
	{
		InitTLS();
	}
	~ThreadPool()
	{
		Join();
	}
public:
	void Launch(function<void()> callback)
	{
		LockGuard guard(_m);
		_threads.emplace_back(thread([=]()
			{
				InitTLS();
				callback();
				DestroyTLS();
			}));
	}
	void Join()
	{
		for (thread& t : _threads) {
			if (t.joinable())
				t.join();
		}
		_threads.clear();
	}
public:
	static void InitTLS()
	{
		static Atomic<uint32> STHreadId = 0;
		LThreadId = STHreadId.fetch_add(1);
	}
	static void DestroyTLS()
	{

	}

	static void DoGlobalQueueWork()
	{
		while (true) {
			uint64 now = ::GetTickCount64();
			if (now > LEndTickCount)
				break;

			JobQueueRef jobQueue = GGlobalQueue->Pop();
			if (jobQueue == nullptr)
				break;

			jobQueue->Execute();
		}
	}
	static void DistributeReservedJobs()
	{
		const uint64 now = ::GetTickCount64();
		GJobTimer->Distribute(now);
	}
private:
	Mutex _m;
	vector<thread> _threads;
};

