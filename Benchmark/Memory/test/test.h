#include <iostream>
#include <chrono>
#include <cstdlib>
#include <cstring>
#include <xmmintrin.h> // SSE
#include <malloc.h>    // _aligned_malloc

constexpr size_t N = 10'000'000;

// 단순한 벡터 연산: dst[i] = src[i] * 1.5f + 2.0f
void vector_op(float* dst, const float* src, size_t count) {
    for (size_t i = 0; i < count; ++i) {
        dst[i] = src[i] * 1.5f + 2.0f;
    }
}

void run_benchmark(const char* label, float* src, float* dst) {
    using namespace std::chrono;
    auto start = high_resolution_clock::now();

    vector_op(dst, src, N);

    auto end = high_resolution_clock::now();
    auto duration = duration_cast<milliseconds>(end - start).count();
    std::cout << label << " -> " << duration << " ms" << std::endl;
}

int malloc_aligned_test() {
    // 비정렬 malloc
    float* src_unaligned = (float*)malloc(sizeof(float) * N);
    float* dst_unaligned = (float*)malloc(sizeof(float) * N);

    // 16바이트 정렬된 malloc
    float* src_aligned = (float*)_aligned_malloc(sizeof(float) * N, 16);
    float* dst_aligned = (float*)_aligned_malloc(sizeof(float) * N, 16);

    // 초기화
    for (size_t i = 0; i < N; ++i) {
        src_unaligned[i] = static_cast<float>(i % 1000);
        src_aligned[i] = src_unaligned[i];
    }

    run_benchmark("Unaligned malloc", src_unaligned, dst_unaligned);
    run_benchmark("Aligned (16-byte)", src_aligned, dst_aligned);

    // 메모리 해제
    free(src_unaligned);
    free(dst_unaligned);
    _aligned_free(src_aligned);
    _aligned_free(dst_aligned);

    return 0;
}
