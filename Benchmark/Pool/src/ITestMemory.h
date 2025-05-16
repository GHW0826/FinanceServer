#pragma once

class ITestMemory abstract
{
public:
    virtual void* Allocate() = 0;
    virtual void Release() = 0;
};
