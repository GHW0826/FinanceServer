#pragma once

struct JobData
{
	JobData(weak_ptr<JobQueue> owner, JobRef job)
		: owner(owner), job(job)
	{
	}

	weak_ptr<JobQueue> owner;
	JobRef job;
};

struct TimerItem
{
	bool operator<(const TimerItem& rhs) const
	{
		return executeTick > rhs.executeTick;
	}

	uint64 executeTick = 0;
	JobData* jobData = nullptr;
};


class JobTimer
{
public:
	void Reserve(uint64 tickAfter, weak_ptr<JobQueue> owner, JobRef job)
	{
		const uint64 executeTick = ::GetTickCount64() + tickAfter;
		JobData* jobData = ObjectPool<JobData>::Pop(owner, job);

		WRITE_LOCK;
		_items.push(TimerItem{ executeTick, jobData });
	}

	void Distribute(uint64 now)
	{
		// 한번에 1 스레드만 통과
		if (_distributing.exchange(true) == true)
			return;

		Vector<TimerItem> items;
		{
			WRITE_LOCK;
			while (_items.empty() == false) {
				const TimerItem& timerItem = _items.top();
				if (now < timerItem.executeTick) {
					break;
				}

				items.push_back(timerItem);
				_items.pop();
			}
		}

		for (TimerItem& item : items) {
			if (JobQueueRef owner = item.jobData->owner.lock()) {
				owner->Push(item.jobData->job);
			}
			ObjectPool<JobData>::Push(item.jobData);
		}

		_distributing.store(false);
	}
	void Clear()
	{
		WRITE_LOCK;
		while (_items.empty() == false) {
			const TimerItem& timerItem = _items.top();
			ObjectPool<JobData>::Push(timerItem.jobData);
			_items.pop();
		}
	}

private:
	USE_LOCK;
	PriorityQueue<TimerItem> _items;
	Atomic<bool> _distributing = false;
};

