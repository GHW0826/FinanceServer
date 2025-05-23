#include "pch.h"
#include "DeadLockProfiler.h"

void DeadLockProfiler::PushLock(const char* name)
{
	LockGuard guard(_lock);

	// 아이디 찾거나 발급
	int32 lockId = 0;
	auto findIt = _nameToId.find(name);
	if (findIt == _nameToId.end()) {
		lockId = static_cast<int32>(_nameToId.size());
		_nameToId[name] = lockId;
		_idToName[lockId] = name;
	}
	else {
		lockId = findIt->second;
	}

	// 잡고 있는 락이 있었다면
	if (LLockStack.empty() == false) {
		// 기존에 발견되지 않은 케이스라면 데드락 여부 다시 확인.
		const int32 prevId = LLockStack.top();
		if (lockId != prevId) {
			set<int32>& history = _lockHistory[prevId];
			if (history.find(lockId) == history.end()) {
				history.insert(lockId);
				CheckCycle();
			}
		}

	}
	LLockStack.push(lockId);
}

void DeadLockProfiler::PopLock(const char* name)
{
	LockGuard guard(_lock);

	if (LLockStack.empty())
		CRASH("MULTIPLE_UNLOCK");

	int32 lockId = _nameToId[name];
	if (LLockStack.top() != lockId)
		CRASH("INVALID_UNLOCK_ORDER");

	LLockStack.pop();
}

void DeadLockProfiler::CheckCycle()
{
	const int32 lockCount = static_cast<int32>(_nameToId.size());
	_discoveredOrder = vector<int32>(lockCount, -1);
	_discoveredCount = 0;
	_finished = vector<bool>(lockCount, false);
	_parent = vector<int32>(lockCount, -1);

	for (int32 lockId = 0; lockId < lockCount; ++lockId)
		Dfs(lockId);

	_discoveredOrder.clear();
	_finished.clear();
	_parent.clear();
}

void DeadLockProfiler::Dfs(int32 here)
{
	if (_discoveredOrder[here] != -1)
		return;

	_discoveredOrder[here] = _discoveredCount++;

	// 모든 인접한 정점을 순회
	auto findIt = _lockHistory.find(here);
	if (findIt == _lockHistory.end()) {
		_finished[here] = true;
		return;
	}

	set<int32>& nextSet = findIt->second;
	for (int32 there : nextSet) {
		// 아직 방문한 적이 없다면 방문
		if (_discoveredOrder[there] == -1) {
			_parent[there] = here;
			Dfs(there);
			continue;
		}

		// here가 therer보다 먼저 발견되었다면 there는 here의 자손. (순방향)
		if (_discoveredOrder[here] < _discoveredOrder[there])
			continue;
		
		// 순방향 아니고 Dfs(there)가 아직 종료하지 않았다면, there는 here의 선조(역방향)
		if (_finished[there] == false) {
			cout << _idToName[here] << " -> " << _idToName[there] << endl;
			
			int32 now = here;
			while (true) {
				cout << _idToName[_parent[now]] << " -> " << _idToName[now] << endl;
				now = _parent[now];
				if (now == there)
					break;
			}
			CRASH("DEADLOCK_DETECTED");
		}
	}
}
