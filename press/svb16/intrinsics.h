#ifndef INTRINSICS_H
#define INTRINSICS_H

#include "common.h"  // architecture macros

#if defined(_MSC_VER)
#include <intrin.h>
#elif defined(__GNUC__) && defined(SVB16_X64)
#include <x86intrin.h>
#elif defined(__GNUC__) && defined(__ARM_NEON__)
#include <arm_neon.h>
#endif

#include <stdint.h>

/*[[gnu::target("sse2")]]*/ inline __m128i m128i_from_bytes(uint8_t a,
                                                                  uint8_t b,
                                                                  uint8_t c,
                                                                  uint8_t d,
                                                                  uint8_t e,
                                                                  uint8_t f,
                                                                  uint8_t g,
                                                                  uint8_t h,
                                                                  uint8_t i,
                                                                  uint8_t j,
                                                                  uint8_t k,
                                                                  uint8_t l,
                                                                  uint8_t m,
                                                                  uint8_t n,
                                                                  uint8_t o,
                                                                  uint8_t p) {
#ifdef _MSC_VER
    return _mm_set_epi32((char)a, (char)b, (char)c, (char)d, (char)e, (char)f, (char)g, (char)h,
                   (char)i, (char)j, (char)k, (char)l, (char)m, (char)n, (char)o, (char)p);
#else
    return _mm_set_epi32(
            (int64_t)((uint64_t)(h) << 56) + ((int64_t)(g) << 48) +
                    ((int64_t)(f) << 40) + ((int64_t)(e) << 32) +
                    ((int64_t)(d) << 24) + ((int64_t)(c) << 16) +
                    ((int64_t)(b) << 8) + (int64_t)(a),
            (int64_t)((uint64_t)(h) << 56) + ((int64_t)(g) << 48) +
                    ((int64_t)(f) << 40) + ((int64_t)(e) << 32) +
                    ((int64_t)(d) << 24) + ((int64_t)(c) << 16) +
                    ((int64_t)(b) << 8) + (int64_t)(a),0,0);
#endif
}

#endif /* INTRINSICS_H */
