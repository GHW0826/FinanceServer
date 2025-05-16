#pragma once

class ITestMemoryPool abstract
{
public:
    virtual void Push() = 0;
    virtual void* Pop() = 0;
};
