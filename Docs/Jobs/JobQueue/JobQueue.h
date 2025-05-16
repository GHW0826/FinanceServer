#pragma once
#include "Job.h"
#include "LockQueue.h"
#include "JobTimer.h"

class JobQueue : public enable_shared_from_this<JobQueue>
{
public:
	void DoAsync(Callback&& callback)
	{
		Push(ObjectPool<Job>::MakeShared(std::move(callback)));
	}

	template<typename T, typename Ret, typename... Args>
	void DoAsync(Ret(T::*memFunc)(Args...), Args... args)
	{
		shared_ptr<T> owner = static_pointer_cast<T>(shared_from_this());
		auto job = ObjectPool<Job>::MakeShared(owner, memFunc, std::forward<Args>(args)...);
		Push(job);
	}

	void DoTimer(uint64 tickAfter, Callback&& callback)
	{
		JobRef job = ObjectPool<Job>::MakeShared(std::move(callback));
		GJobTimer->Reserve(tickAfter, shared_from_this(), job);
	}

	template<typename T, typename Ret, typename... Args>
	void DoTimer(uint64 tickAfter, Ret(T::* memFunc)(Args...), Args... args)
	{
		shared_ptr<T> owner = static_pointer_cast<T>(shared_from_this());
		auto job = ObjectPool<Job>::MakeShared(owner, memFunc, std::forward<Args>(args)...);
		GJobTimer->Reserve(tickAfter, shared_from_this(), job);
	}

	void ClearJob() {
		_jobs.Clear();
	}
	void Execute()
	{
		LCurrentJobQueue = this;

		while (true) {
			Vector<JobRef> jobs;
			_jobs.PopAll(OUT jobs);

			const int32 jobCount = static_cast<int32>(jobs.size());
			for (int32 i = 0; i < jobCount; ++i) {
				jobs[i]->Execute();
			}

			if (_jobCount.fetch_sub(jobCount) == jobCount) {
				LCurrentJobQueue = nullptr;
				return;
			}

			const uint64 now = ::GetTickCount64();
			if (now > LEndTickCount) {
				LCurrentJobQueue = nullptr;
				GGlobalQueue->Push(shared_from_this());
			}
		}
	}
	void Push(JobRef job, bool pushOnly = false)
	{
		const int32 prevCount = _jobCount.fetch_add(1);
		_jobs.Push(job); // WRITE_LOCK

		// 첫번째 job을 넣은스레드가 실행까지 할당
		if (prevCount == 0) {
			// 이미 실행중인 JobQUeue가 없으면 실행
			if (LCurrentJobQueue == nullptr && pushOnly == false) {
				Execute();
			}
			else {
				// 여유 있는 다른 스레드가 실행하도록 GlobalQeue에 넘긴다
				GGlobalQueue->Push(shared_from_this());
			}
		}
	}

protected:
	LockQueue<JobRef> _jobs;
	Atomic<int32> _jobCount = 0;
};

