#pragma once
#include <cstdlib>
#include <vector>
#include <mutex>
#include <stdexcept>

class ListMemoryChunk
{
public:
    // blockSize : 고정 블록 크기(바이트)
    // blockCount : 준비할 블록 개수
    ListMemoryChunk(std::size_t blockSize, std::size_t blockCount)
        : blockSize_(blockSize), blockCount_(blockCount)
    {
        if (blockSize_ < sizeof(void*)) {
            // 블록 크기가 최소한 포인터 크기보다 작으면 연결 리스트를 담기 곤란
            throw std::runtime_error("blockSize too small");
        }
        if (blockCount_ == 0) {
            throw std::runtime_error("blockCount must be > 0");
        }

        // 1) 전체 청크 한 번에 할당
        poolMemory_ = static_cast<unsigned char*>(std::malloc(blockSize_ * blockCount_));
        if (!poolMemory_) {
            throw std::bad_alloc();
        }

        // 2) free list 초기화
        // 모든 블록을 단일 연결 리스트로 연결
        freeList_ = nullptr;
        for (std::size_t i = 0; i < blockCount_; i++) {
            void* blockAddr = poolMemory_ + i * blockSize_;
            // block의 첫 부분에 "next" 포인터(=void*)를 저장 가능
            pushFreeBlock(blockAddr);
        }
    }

    ~ListMemoryChunk()
    {
        // 생성 시 malloc() 했으니 free()로 해제
        // 아직 사용 중인 블록이 있어도 전체가 한 번에 해제됨
        if (poolMemory_) {
            std::free(poolMemory_);
            poolMemory_ = nullptr;
        }
    }

    // 할당 : free list에서 블록 하나 꺼내 반환
    void* allocate()
    {
        std::lock_guard<std::mutex> lock(mutex_);

        if (!freeList_) {
            // 여유 블록 없음 => 예외 or fallback
            throw std::bad_alloc();
        }

        // freeList_가 가리키는 블록을 꺼냄
        void* block = freeList_;
        // block의 첫 부분에 "next" 포인터가 있다고 가정
        freeList_ = *reinterpret_cast<void**>(block);
        return block;
    }

    // 해제 : block을 free list에 되돌림
    void deallocate(void* block)
    {
        std::lock_guard<std::mutex> lock(mutex_);
        pushFreeBlock(block);
    }

    std::size_t getBlockSize() const { return blockSize_; }
    std::size_t getBlockCount() const { return blockCount_; }

private:
    void pushFreeBlock(void* block)
    {
        // block의 첫 부분에 'freeList_'를 저장 -> next 포인터 역할
        *reinterpret_cast<void**>(block) = freeList_;
        // freeList_ 갱신
        freeList_ = block;
    }

private:
    std::mutex mutex_;               // 멀티스레드 안전을 위해 사용
    std::size_t blockSize_;          // 고정 블록 크기
    std::size_t blockCount_;         // 전체 블록 개수
    unsigned char* poolMemory_;      // 전체 할당된 메모리 시작 주소
    void* freeList_;                 // 단일 연결 리스트의 head 포인터
};
