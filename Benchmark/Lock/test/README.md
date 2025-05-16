# Lock Test

<details>
<summary>ShortCS</summary>

```cpp
template<typename T>
void ShortCriticalSection(T& lock, int threadOps) 
{
	for (int i = 0; i < threadOps; ++i) {
		WriteLockGuard wlg(lock);
		sharedData++;
	}
}
```
</details>   

| Lock Type     | ShortCS Time (Thread 1) | ShortCS Time (Thread 4) | ShortCS Time (Thread 8) | ShortCS Time (Thread 16) | ShortCS Time (Thread 25) |
|-----------------------------|--------------|-------------|----------------|----------------|----------------|
| std::mutex                  |  XX us       |  XXXX us    |      -         |      -         |      -         |
| std::shared_mutex           |  XX us       |  XXXX us    |      -         |      -         |      -         |
| std::recursive_mutex        |     -        |     -       |   XXXX us      |   XXXX us      |   XXXX us      |
| Critical Section(500)       |     -        |     -       |   XXXX us      |   XXXX us      |   XXXX us      |
| Critical Section(1000)      |     -        |     -       |   XXXX us      |   XXXX us      |   XXXX us      |
| Critical Section(2000)      |     -        |     -       |   XXXX us      |   XXXX us      |   XXXX us      |
| Critical Section(5000)      |     -        |     -       |   XXXX us      |   XXXX us      |   XXXX us      |
| Critical Section(10000)     |     -        |     -       |   XXXX us      |   XXXX us      |   XXXX us      |
| Event                       |     -        |     -       |   XXXX us      |   XXXX us      |   XXXX us      |
| 커널 Mutex                  |     -        |     -       |   XXXX us      |   XXXX us      |   XXXX us      |
| SRWLOCK                     |     -        |     -       |   XXXX us      |   XXXX us      |   XXXX us      |
| SpinLock                    |     -        |     -       |   XXXX us      |   XXXX us      |   XXXX us      |
| SpinLock and Yield(500)     |     -        |     -       |   XXXX us      |   XXXX us      |   XXXX us      |
| SpinLock and Yield(1000)    |     -        |     -       |   XXXX us      |   XXXX us      |   XXXX us      |
| SpinLock and Yield(2000)    |     -        |     -       |   XXXX us      |   XXXX us      |   XXXX us      |
| SpinLock and Yield(5000)    |     -        |     -       |   XXXX us      |   XXXX us      |   XXXX us      |
| SpinLock and Yield(10000)   |     -        |     -       |   XXXX us      |   XXXX us      |   XXXX us      |
| SpinLock and Event(500)     |     -        |     -       |   XXXX us      |   XXXX us      |   XXXX us      |
| SpinLock and Event(1000)    |     -        |     -       |   XXXX us      |   XXXX us      |   XXXX us      |
| SpinLock and Event(2000)    |     -        |     -       |   XXXX us      |   XXXX us      |   XXXX us      |
| SpinLock and Event(5000)    |     -        |     -       |   XXXX us      |   XXXX us      |   XXXX us      |
| SpinLock and Event(10000)   |     -        |     -       |   XXXX us      |   XXXX us      |   XXXX us      |

<details>
<summary>LongCS</summary>

```cpp
template<typename T>
void LongCriticalSection(T& lock, int threadOps) 
{
	for (int i = 0; i < threadOps; ++i) {
		WriteLockGuard wlg(lock);
		std::this_thread::sleep_for(std::chrono::microseconds(100));
	}
}
```
</details>   

| Lock Type     | LongCS Time (Thread 1) | LongCS Time (Thread 4) | LongCS Time (Thread 8) | LongCS Time (Thread 16) | LongCS Time (Thread 25) |
|-----------------------------|--------------|-------------|----------------|----------------|----------------|
| std::mutex                  |  XX us       |  XXXX us    |      -         |      -         |      -         |
| std::shared_mutex           |  XX us       |  XXXX us    |      -         |      -         |      -         |
| std::recursive_mutex        |     -        |     -       |   XXXX us      |   XXXX us      |   XXXX us      |
| Critical Section(500)       |     -        |     -       |   XXXX us      |   XXXX us      |   XXXX us      |
| Critical Section(1000)      |     -        |     -       |   XXXX us      |   XXXX us      |   XXXX us      |
| Critical Section(2000)      |     -        |     -       |   XXXX us      |   XXXX us      |   XXXX us      |
| Critical Section(5000)      |     -        |     -       |   XXXX us      |   XXXX us      |   XXXX us      |
| Critical Section(10000)     |     -        |     -       |   XXXX us      |   XXXX us      |   XXXX us      |
| Event                       |     -        |     -       |   XXXX us      |   XXXX us      |   XXXX us      |
| 커널 Mutex                  |     -        |     -       |   XXXX us      |   XXXX us      |   XXXX us      |
| SRWLOCK                     |     -        |     -       |   XXXX us      |   XXXX us      |   XXXX us      |
| SpinLock                    |     -        |     -       |   XXXX us      |   XXXX us      |   XXXX us      |
| SpinLock and Yield(500)     |     -        |     -       |   XXXX us      |   XXXX us      |   XXXX us      |
| SpinLock and Yield(1000)    |     -        |     -       |   XXXX us      |   XXXX us      |   XXXX us      |
| SpinLock and Yield(2000)    |     -        |     -       |   XXXX us      |   XXXX us      |   XXXX us      |
| SpinLock and Yield(5000)    |     -        |     -       |   XXXX us      |   XXXX us      |   XXXX us      |
| SpinLock and Yield(10000)   |     -        |     -       |   XXXX us      |   XXXX us      |   XXXX us      |
| SpinLock and Event(500)     |     -        |     -       |   XXXX us      |   XXXX us      |   XXXX us      |
| SpinLock and Event(1000)    |     -        |     -       |   XXXX us      |   XXXX us      |   XXXX us      |
| SpinLock and Event(2000)    |     -        |     -       |   XXXX us      |   XXXX us      |   XXXX us      |
| SpinLock and Event(5000)    |     -        |     -       |   XXXX us      |   XXXX us      |   XXXX us      |
| SpinLock and Event(10000)   |     -        |     -       |   XXXX us      |   XXXX us      |   XXXX us      |

<details>
<summary>RWCS</summary>

```cpp
template<typename T>
void rwScenario(T& rwLock, int threadOps, std::vector<bool>& rwOrder)
{
    for (int i = 0; i < threadOps; ++i) {
        if (rwOrder[i] == true) {
            ReadLockGuard rlg(rwLock);
            int val = sharedData;
            (void)val;
            // std::this_thread::sleep_for(std::chrono::microseconds(100));
        }
        else {
            WriteLockGuard wlg(rwLock);
            ++sharedData;
            //std::this_thread::sleep_for(std::chrono::microseconds(100));
        }
    }
}
```
</details>   

| Lock Type     | RWCS Time (Thread 1) | RWCS Time (Thread 4) | RWCS Time (Thread 8) | RWCS Time (Thread 16) | RWCS Time (Thread 25) |
|-----------------------------|--------------|-------------|----------------|----------------|----------------|
| std::mutex                  |  XX us       |  XXXX us    |      -         |      -         |      -         |
| std::shared_mutex           |  XX us       |  XXXX us    |      -         |      -         |      -         |
| std::recursive_mutex        |     -        |     -       |   XXXX us      |   XXXX us      |   XXXX us      |
| Critical Section(500)       |     -        |     -       |   XXXX us      |   XXXX us      |   XXXX us      |
| Critical Section(1000)      |     -        |     -       |   XXXX us      |   XXXX us      |   XXXX us      |
| Critical Section(2000)      |     -        |     -       |   XXXX us      |   XXXX us      |   XXXX us      |
| Critical Section(5000)      |     -        |     -       |   XXXX us      |   XXXX us      |   XXXX us      |
| Critical Section(10000)     |     -        |     -       |   XXXX us      |   XXXX us      |   XXXX us      |
| Event                       |     -        |     -       |   XXXX us      |   XXXX us      |   XXXX us      |
| 커널 Mutex                  |     -        |     -       |   XXXX us      |   XXXX us      |   XXXX us      |
| SRWLOCK                     |     -        |     -       |   XXXX us      |   XXXX us      |   XXXX us      |
| SpinLock                    |     -        |     -       |   XXXX us      |   XXXX us      |   XXXX us      |
| SpinLock and Yield(500)     |     -        |     -       |   XXXX us      |   XXXX us      |   XXXX us      |
| SpinLock and Yield(1000)    |     -        |     -       |   XXXX us      |   XXXX us      |   XXXX us      |
| SpinLock and Yield(2000)    |     -        |     -       |   XXXX us      |   XXXX us      |   XXXX us      |
| SpinLock and Yield(5000)    |     -        |     -       |   XXXX us      |   XXXX us      |   XXXX us      |
| SpinLock and Yield(10000)   |     -        |     -       |   XXXX us      |   XXXX us      |   XXXX us      |
| SpinLock and Event(500)     |     -        |     -       |   XXXX us      |   XXXX us      |   XXXX us      |
| SpinLock and Event(1000)    |     -        |     -       |   XXXX us      |   XXXX us      |   XXXX us      |
| SpinLock and Event(2000)    |     -        |     -       |   XXXX us      |   XXXX us      |   XXXX us      |
| SpinLock and Event(5000)    |     -        |     -       |   XXXX us      |   XXXX us      |   XXXX us      |
| SpinLock and Event(10000)   |     -        |     -       |   XXXX us      |   XXXX us      |   XXXX us      |

<details>
<summary>1. std::mutex</summary>

```cpp
class StdLock : public ILockTest
{
public:
    void lock_shared() override
    {
        _m.lock();
    }
    void unlock_shared() override
    {
        _m.unlock();
    }
    void lock_exclusive() override
    {
        _m.lock();
    }
    void unlock_exclusive() override
    {
        _m.unlock();
    }
private:
    std::mutex _m;
};
```

</details>   
<details>
<summary>2. std::shared_mutex(C++17)</summary>

```cpp
class StdSharedLock : public ILockTest
{
public:
    void lock_shared() override
    {
        _sm.lock_shared();
    }
    void unlock_shared() override
    {
        _sm.unlock_shared();
    }
    void lock_exclusive() override
    {
        _sm.lock();
    }
    void unlock_exclusive() override
    {
        _sm.unlock();
    }
public:
    std::shared_mutex _sm;
};
```

</details>   

<details>
<summary>3. std::recursive_mutex</summary>

```cpp
class StdRecursiveLock : public ILockTest
{
public:
    void lock_shared() override
    {
        _m.lock();
    }
    void unlock_shared() override
    {
        _m.unlock();
    }
    void lock_exclusive() override
    {
        _m.lock();
    }
    void unlock_exclusive() override
    {
        _m.unlock();
    }
private:
    std::recursive_mutex _m;
};
```

</details>   
<details>
<summary>4. Critical Section</summary>
	
```cpp
class CriticalSectionLock : public ILockTest
{
public:
    CriticalSectionLock()
    {
        if (InitializeCriticalSectionAndSpinCount(&_cs, 2000) == false) {
            std::terminate();
        }
    }
    ~CriticalSectionLock()
    {
        DeleteCriticalSection(&_cs);
    }
public:
    void lock_shared() override
    {
        EnterCriticalSection(&_cs);
    }
    void unlock_shared() override
    {
        LeaveCriticalSection(&_cs);
    }
    void lock_exclusive() override
    {
        EnterCriticalSection(&_cs);

    }
    void unlock_exclusive() override
    {
        LeaveCriticalSection(&_cs);
    }
private:
    CRITICAL_SECTION  _cs;
};
```
</details>   
<details>
<summary>5. Event</summary>
	
```cpp
class EventLock : public ILockTest
{
public:
    EventLock() {
        _hEvent = CreateEvent(nullptr, FALSE, TRUE, nullptr);
        if (_hEvent == nullptr) {
            std::terminate();
        }
    }
    ~EventLock() {
        CloseHandle(_hEvent);
    }
public:
    void lock_shared() override {
        DWORD dwWait = WaitForSingleObject(_hEvent, INFINITE);
        if (dwWait != WAIT_OBJECT_0) {
            std::terminate();
        }
    }

    void unlock_shared() override {
        if (!SetEvent(_hEvent)) {
            std::terminate();
        }
    }
    void lock_exclusive() override {
        DWORD dwWait = WaitForSingleObject(_hEvent, INFINITE);
        if (dwWait != WAIT_OBJECT_0) {
            std::terminate();
        }
    }
    void unlock_exclusive() override {
        if (!SetEvent(_hEvent)) {
            std::cerr << "SetEvent failed: " << GetLastError() << "\n";
            std::terminate();
        }
    }
private:
    HANDLE _hEvent;
};
```
</details>   
<details>
<summary>6. 커널 Mutex</summary>
	
```cpp
class MutexLock : public ILockTest
{
public:
    MutexLock() {
        _hmut = CreateMutex(nullptr, FALSE, nullptr);
        if (_hmut == nullptr) {
            std::terminate();
        }
    }
    ~MutexLock() {
        CloseHandle(_hmut);
    }
public:
    void lock_shared() override {
        DWORD dwWait = WaitForSingleObject(_hmut, INFINITE);
        if (dwWait != WAIT_OBJECT_0) {
            std::terminate();
        }
    }
    void unlock_shared() override {
        if (ReleaseMutex(_hmut) == false) {
            std::terminate();
        }
    }
    void lock_exclusive() override {
        DWORD dwWait = WaitForSingleObject(_hmut, INFINITE);
        if (dwWait != WAIT_OBJECT_0) {
            std::terminate();
        }
    }
    void unlock_exclusive() override {
        if (ReleaseMutex(_hmut) == false) {
            std::terminate();
        }
    }
private:
    HANDLE _hmut;
};
```
</details>   
<details>
<summary>7. SpinLock</summary>

```cpp
class SpinLock : public ILockTest
{
public:
    void lock_shared() override
    {
        bool expected = false;
        while (!_flag.compare_exchange_strong(expected, true)) {
            expected = false;
        }
    }
    void unlock_shared() override
    {
        _flag.store(false);
    }
    void lock_exclusive() override
    {
        bool expected = false;
        while (!_flag.compare_exchange_strong(expected, true)) {
            expected = false;
        }
    }
    void unlock_exclusive() override
    {
        _flag.store(false);
    }
private:
    std::atomic<bool> _flag = false;
};
```
</details>   
<details>
<summary>8. SpinLock and Yield</summary>

```cpp
class SYLock : public ILockTest
{
	enum :int {
		MAX_SPIN_COUNT = 1'000,
	};
public:
	void lock_shared() override
	{
		bool expected = false;
		while (true) {
			for (int spinCount = 0; spinCount < MAX_SPIN_COUNT; ++spinCount) {
				if (_flag.compare_exchange_strong(expected, true)) {
					return;
				}
			}
			this_thread::yield();
		}
	}
	void unlock_shared() override
	{
		_flag.store(false);
	}
	void lock_exclusive() override
	{
		while (true) {
			for (int spinCount = 0; spinCount < MAX_SPIN_COUNT; ++spinCount) {
				bool expected = false;
				if (_flag.compare_exchange_strong(expected, true)) {
					return;
				}
			}
			this_thread::yield();
		}
	}
	void unlock_exclusive() override
	{
		_flag.store(false);
	}
private:
	std::atomic<bool> _flag = false;
};
```
</details>   
<details>
<summary>9. SpinLock and Event</summary>

```cpp
class SELock : public ILockTest
{
	enum :int {
		MAX_SPIN_COUNT = 1'000,
	};
public:
	SELock()
	{
		_hEvent = CreateEvent(nullptr, FALSE, TRUE, nullptr);
		if (_hEvent == nullptr) {
			std::cerr << "CreateEvent failed: " << GetLastError() << "\n";
			std::terminate();
		}
	}
	~SELock() {
		CloseHandle(_hEvent);
	}
public:
	void lock_shared() override
	{
		bool expected = false;
		while (true) {
			for (int spinCount = 0; spinCount < MAX_SPIN_COUNT; ++spinCount) {
				if (_flag.compare_exchange_strong(expected, true)) {
					return;
				}
			}

			DWORD dwWait = WaitForSingleObject(_hEvent, INFINITE);
			if (dwWait != WAIT_OBJECT_0) {
				std::cerr << "WaitForSingleObject failed: " << GetLastError() << "\n";
				std::terminate();
			}
		}
	}
	void unlock_shared() override
	{
		_flag.store(false);
		if (SetEvent(_hEvent) == false) {
			std::cerr << "SetEvent failed: " << GetLastError() << "\n";
			std::terminate();
		}
	}
	void lock_exclusive() override
	{
		while (true) {
			for (int spinCount = 0; spinCount < MAX_SPIN_COUNT; ++spinCount) {
				bool expected = false;
				if (_flag.compare_exchange_strong(expected, true)) {
					return;
				}
			}

			DWORD dwWait = WaitForSingleObject(_hEvent, INFINITE);
			if (dwWait != WAIT_OBJECT_0) {
				std::cerr << "WaitForSingleObject failed: " << GetLastError() << "\n";
				std::terminate();
			}
		}
	}
	void unlock_exclusive() override
	{
		_flag.store(false);
		if (SetEvent(_hEvent) == false) {
			std::cerr << "SetEvent failed: " << GetLastError() << "\n";
			std::terminate();
		}
	}
private:
	std::atomic<bool> _flag = false;
	HANDLE _hEvent;
};
```
</details>   
