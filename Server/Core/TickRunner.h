#pragma once
#include <functional>

using UpdateFunc = std::function<void()>;

template<typename T>
class TickRunner 
{
public:
    TickRunner(std::shared_ptr<T> updatable, int tickMs)
        : _updatable(updatable), _tickMs(tickMs), _running(true)
    {
        Run();
    }
    ~TickRunner() {
        Stop();
    }
public:
    void Stop() {
        _running = false;
    }

private:
    void Run() {
        using namespace std::chrono;
        while (_running) {
            auto start = steady_clock::now();
            
            _updatable->Update();

            auto end = steady_clock::now();

            auto elapsed = duration_cast<milliseconds>(end - start).count();
            auto sleepTime = _tickMs - elapsed;
            if (sleepTime > 0)
                std::this_thread::sleep_for(milliseconds(sleepTime));
        }
    }

    std::shared_ptr<T> _updatable;
    int _tickMs;
    std::atomic<bool> _running;
};
