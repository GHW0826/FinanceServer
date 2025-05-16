#pragma once
#include <cstdlib>
#include <stack>
#include <mutex>
#include <stdexcept>

class StackMemoryPool
{
public:
    StackMemoryPool(std::size_t blockSize, std::size_t blockCount)
        : blockSize_(blockSize), blockCount_(blockCount)
    {
        if (blockSize_ < sizeof(void*)) {
            throw std::runtime_error("blockSize is too small");
        }
        if (blockCount_ == 0) {
            throw std::runtime_error("blockCount must be > 0");
        }

        // 한 번에 할당
        poolMemory_ = static_cast<unsigned char*>(
            std::malloc(blockSize_ * blockCount_)
            );
        if (!poolMemory_) {
            throw std::bad_alloc();
        }

        // 모든 블록을 stack에 push (LIFO)
        for (std::size_t i = 0; i < blockCount_; i++) {
            void* blockAddr = poolMemory_ + i * blockSize_;
            freeStack_.push(blockAddr);
        }
    }

    ~StackMemoryPool()
    {
        if (poolMemory_) {
            std::free(poolMemory_);
            poolMemory_ = nullptr;
        }
    }

    void* allocate()
    {
        std::lock_guard<std::mutex> lock(mutex_);
        if (freeStack_.empty()) {
            throw std::bad_alloc();
        }
        void* block = freeStack_.top();
        freeStack_.pop();
        return block;
    }

    void deallocate(void* ptr)
    {
        std::lock_guard<std::mutex> lock(mutex_);
        freeStack_.push(ptr);
    }

    std::size_t getBlockSize()  const { return blockSize_; }
    std::size_t getBlockCount() const { return blockCount_; }

private:
    std::mutex mutex_;
    std::size_t blockSize_;
    std::size_t blockCount_;
    unsigned char* poolMemory_;

    // LIFO: std::stack<void*>
    std::stack<void*> freeStack_;
};
