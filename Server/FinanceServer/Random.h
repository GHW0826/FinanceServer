#pragma once
#include <random>
#include <thread>
#include <chrono>

class Random
{
public:
    // 스레드마다 고유한 mt19937 생성기
    static std::mt19937& GetRNG()
    {
        thread_local std::mt19937 rng(Seed());
        return rng;
    }

    static int NextInt(int min, int max)
    {
        std::uniform_int_distribution<int> dist(min, max);
        return dist(GetRNG());
    }

    static float NextFloat(float min = 0.0f, float max = 1.0f)
    {
        std::uniform_real_distribution<float> dist(min, max);
        return dist(GetRNG());
    }

    static bool NextBool()
    {
        return NextInt(0, 1) == 1;
    }

private:
    // 시드 생성 (시계 + 스레드 ID)
    static uint32_t Seed()
    {
        auto now = std::chrono::high_resolution_clock::now().time_since_epoch().count();
        auto tid = std::hash<std::thread::id>()(std::this_thread::get_id());
        return static_cast<uint32_t>(now ^ tid);
    }
};