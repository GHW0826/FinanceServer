#pragma once

class GlobalQueue
{
public:
	GlobalQueue()
	{
	}
	~GlobalQueue()
	{
	}
	void Push(JobQueueRef jobQueue)
	{
		_jobQueue.Push(jobQueue);
	}
	JobQueueRef Pop()
	{
		return _jobQueue.Pop();
	}
private:
	LockQueue<JobQueueRef> _jobQueue;
};

