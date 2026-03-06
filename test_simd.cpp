#include <immintrin.h>
#include <iostream>
#include <iomanip>
#include <vector>
#include <cstdint>

using namespace std;

int main() {
    cout << "Testing AVX2 4-pixel parallel resize simulation..." << endl;
    
    // Simulate source pixels
    uint8_t row0[32] = {0};
    uint8_t row1[32] = {0};
    
    // Set 4 source pixels on row0 to RGBA: {10, 20, 30, 255}, {100, 110, 120, 255}, {50, 60, 70, 255}, {200, 210, 220, 255}
    // Set matching ones on row1 to 0
    row0[0] = 10; row0[1] = 20; row0[2] = 30; row0[3] = 255;
    row0[4] = 100; row0[5] = 110; row0[6] = 120; row0[7] = 255;
    row0[8] = 50; row0[9] = 60; row0[10] = 70; row0[11] = 255;
    row0[12] = 200; row0[13] = 210; row0[14] = 220; row0[15] = 255;
    
    // Indices (simulating xCoeff.idx)
    int i00 = 0, i01 = 0;
    int i10 = 4, i11 = 4;
    int i20 = 8, i21 = 8;
    int i30 = 12, i31 = 12;
    
    // Load 4 pixels per variable
    __m128i v_s00 = _mm_set_epi32(*(const uint32_t*)(row0 + i30), *(const uint32_t*)(row0 + i20), *(const uint32_t*)(row0 + i10), *(const uint32_t*)(row0 + i00));
    __m128i v_s01 = _mm_set_epi32(*(const uint32_t*)(row0 + i31), *(const uint32_t*)(row0 + i21), *(const uint32_t*)(row0 + i11), *(const uint32_t*)(row0 + i01));
    __m128i v_s10 = _mm_set_epi32(*(const uint32_t*)(row1 + i30), *(const uint32_t*)(row1 + i20), *(const uint32_t*)(row1 + i10), *(const uint32_t*)(row1 + i00));
    __m128i v_s11 = _mm_set_epi32(*(const uint32_t*)(row1 + i31), *(const uint32_t*)(row1 + i21), *(const uint32_t*)(row1 + i11), *(const uint32_t*)(row1 + i01));

    // Unpack to 32-bit ints
    __m256i p00_lo = _mm256_cvtepu8_epi32(v_s00);
    __m256i p00_hi = _mm256_cvtepu8_epi32(_mm_srli_si128(v_s00, 8));
    __m256i p01_lo = _mm256_cvtepu8_epi32(v_s01);
    __m256i p01_hi = _mm256_cvtepu8_epi32(_mm_srli_si128(v_s01, 8));
    __m256i p10_lo = _mm256_cvtepu8_epi32(v_s10);
    __m256i p10_hi = _mm256_cvtepu8_epi32(_mm_srli_si128(v_s10, 8));
    __m256i p11_lo = _mm256_cvtepu8_epi32(v_s11);
    __m256i p11_hi = _mm256_cvtepu8_epi32(_mm_srli_si128(v_s11, 8));

    // Simulate weights: Let's assume 100% weight to s00
    constexpr int kWeightBits = 11;
    constexpr int kWeightScale = 1 << kWeightBits;      // 2048
    constexpr int kWeightShift = kWeightBits * 2;       // 22
    constexpr int kWeightRound = 1 << (kWeightShift - 1);
    
    int32_t w_full = kWeightScale * kWeightScale; // 100% weight -> 2048 * 2048
    int32_t w_zero = 0;
    
    __m256i w00_lo_v = _mm256_set1_epi32(w_full);
    __m256i w01_lo_v = _mm256_set1_epi32(w_zero);
    __m256i w10_lo_v = _mm256_set1_epi32(w_zero);
    __m256i w11_lo_v = _mm256_set1_epi32(w_zero);
    
    __m256i w00_hi_v = _mm256_set1_epi32(w_full);
    __m256i w01_hi_v = _mm256_set1_epi32(w_zero);
    __m256i w10_hi_v = _mm256_set1_epi32(w_zero);
    __m256i w11_hi_v = _mm256_set1_epi32(w_zero);
    
    __m256i sum_lo = _mm256_mullo_epi32(p00_lo, w00_lo_v);
    sum_lo = _mm256_add_epi32(sum_lo, _mm256_mullo_epi32(p01_lo, w01_lo_v));
    sum_lo = _mm256_add_epi32(sum_lo, _mm256_mullo_epi32(p10_lo, w10_lo_v));
    sum_lo = _mm256_add_epi32(sum_lo, _mm256_mullo_epi32(p11_lo, w11_lo_v));

    __m256i sum_hi = _mm256_mullo_epi32(p00_hi, w00_hi_v);
    sum_hi = _mm256_add_epi32(sum_hi, _mm256_mullo_epi32(p01_hi, w01_hi_v));
    sum_hi = _mm256_add_epi32(sum_hi, _mm256_mullo_epi32(p10_hi, w10_hi_v));
    sum_hi = _mm256_add_epi32(sum_hi, _mm256_mullo_epi32(p11_hi, w11_hi_v));
    
    __m256i vRound = _mm256_set1_epi32(kWeightRound);
    sum_lo = _mm256_srai_epi32(_mm256_add_epi32(sum_lo, vRound), kWeightShift);
    sum_hi = _mm256_srai_epi32(_mm256_add_epi32(sum_hi, vRound), kWeightShift);

    __m256i packed16 = _mm256_packs_epi32(sum_lo, sum_hi);
    __m256i packed8 = _mm256_packus_epi16(packed16, packed16);
    
    // We expect the original values since we used 100% weight: {10, 20, 30, 255}, etc.
    __m128i p0_p2 = _mm256_castsi256_si128(packed8);
    __m128i p1_p3 = _mm256_extracti128_si256(packed8, 1);
    
    uint8_t dst[16] = {0};
    *(uint32_t*)(dst + 0) = _mm_cvtsi128_si32(p0_p2);
    *(uint32_t*)(dst + 4) = _mm_cvtsi128_si32(p1_p3);
    *(uint32_t*)(dst + 8) = _mm_extract_epi32(p0_p2, 1);
    *(uint32_t*)(dst + 12) = _mm_extract_epi32(p1_p3, 1);
    
    for (int i=0; i<4; ++i) {
        cout << "Pixel " << i << ": " 
             << (int)dst[i*4+0] << ", "
             << (int)dst[i*4+1] << ", "
             << (int)dst[i*4+2] << ", "
             << (int)dst[i*4+3] << endl;
    }
    
    cout << "Test completed." << endl;
    return 0;
}
