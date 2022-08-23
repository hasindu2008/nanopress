#include "intrinsics.h"
#include "shuffle_tables.h"
#include "svb16.h"  // svb16_key_length

#include <stddef.h>
#include <stdint.h>
#include <string.h>

#ifdef SVB16_X64

/*[[gnu::target("ssse3")]]*/ inline __m128i zigzag_decode(__m128i val) {
    return _mm_xor_si128(
            // N >> 1
            _mm_srli_epi16(val, 1),
            // 0xFFFF if N & 1 else 0x0000
            _mm_srai_epi16(_mm_slli_epi16(val, 15), 15)
            // alternative: _mm_sign_epi16(ones, _mm_slli_epi16(buf, 15))
    );
}

/*[[gnu::target("ssse3")]]*/ static inline __m128i unpack(uint32_t key, const uint8_t *SVB_RESTRICT *data) {
    uint8_t const len = (uint8_t)(8 + svb16_popcount(key));
    __m128i data_reg = _mm_loadu_si128((__m128i const *)(*data));
    __m128i const shuffle = *(__m128i const *)(&g_decode_shuffle_table[key]);

    data_reg = _mm_shuffle_epi8(data_reg, shuffle);
    *data += len;

    return data_reg;
}

/*[[gnu::target("ssse3")]]*/ inline void store_8(bool UseDelta, bool UseZigzag, int16_t *to, __m128i value, __m128i *prev) {
    SVB16_IF_CONSTEXPR(UseZigzag) { value = zigzag_decode(value); }

    SVB16_IF_CONSTEXPR(UseDelta) {
        __m128i const broadcast_last_16 =
                m128i_from_bytes(14, 15, 14, 15, 14, 15, 14, 15, 14, 15, 14, 15, 14, 15, 14, 15);
        // value == [A B C D E F G H] (16 bit values)
        __m128i add = _mm_slli_si128(value, 2);
        // add   == [- A B C D E F G]
        *prev = _mm_shuffle_epi8(*prev, broadcast_last_16);
        // *prev == [P P P P P P P P]
        value = _mm_add_epi16(value, add);
        // value == [A AB BC CD DE FG GH]
        add = _mm_slli_si128(value, 4);
        // add   == [- - A AB BC CD DE EF]
        value = _mm_add_epi16(value, add);
        // value == [A AB ABC ABCD BCDE CDEF DEFG EFGH]
        add = _mm_slli_si128(value, 8);
        // add   == [- - - - A AB ABC ABCD]
        value = _mm_add_epi16(value, add);
        // value == [A AB ABC ABCD ABCDE ABCDEF ABCDEFG ABCDEFGH]
        value = _mm_add_epi16(value, *prev);
        // value == [PA PAB PABC PABCD PABCDE PABCDEF PABCDEFG PABCDEFGH]
        *prev = value;
    }

    _mm_storeu_si128((__m128i *)(to), value);
}

/*[[gnu::target("sse4.1")]]*/ uint8_t const *decode_sse(bool UseDelta, bool UseZigZag, int16_t *out,
                                                    uint8_t const *SVB_RESTRICT keys,
                                                    uint8_t const *SVB_RESTRICT data,
                                                    uint32_t count,
                                                    int16_t prev) {
    // this code treats all input as uint16_t (except the zigzag code, which treats it as int16_t)
    // this isn't a problem, as the scalar code does the same

    // handle blocks of 32 values
    if (count >= 64) {
        size_t const key_bytes = count / 8;

        __m128i prev_reg;
        SVB16_IF_CONSTEXPR(UseDelta) { prev_reg = _mm_set1_epi16(prev); }

        int64_t offset = -(int64_t)(key_bytes) / 8 + 1;  // 8 -> 4?
        const uint64_t *keyPtr64 = (uint64_t const *)(keys) - offset;
        uint64_t nextkeys;
        memcpy(&nextkeys, keyPtr64 + offset, sizeof(nextkeys));

        __m128i data_reg;

        for (; offset != 0; ++offset) {
            uint64_t keys = nextkeys;
            memcpy(&nextkeys, keyPtr64 + offset + 1, sizeof(nextkeys));
            // faster 16-bit delta since we only have 8-bit values
            if (!keys) {  // 64 1-byte ints in a row

                // _mm_cvtepu8_epi16: SSE4.1
                data_reg =
                        _mm_cvtepu8_epi16(_mm_lddqu_si128((__m128i const *)(data)));
                store_8(UseDelta, UseZigZag, out, data_reg, &prev_reg);
                data_reg = _mm_cvtepu8_epi16(
                        _mm_lddqu_si128((__m128i const *)(data + 8)));
                store_8(UseDelta, UseZigZag, out + 8, data_reg, &prev_reg);
                data_reg = _mm_cvtepu8_epi16(
                        _mm_lddqu_si128((__m128i const *)(data + 16)));
                store_8(UseDelta, UseZigZag, out + 16, data_reg, &prev_reg);
                data_reg = _mm_cvtepu8_epi16(
                        _mm_lddqu_si128((__m128i const *)(data + 24)));
                store_8(UseDelta, UseZigZag, out + 24, data_reg, &prev_reg);
                data_reg = _mm_cvtepu8_epi16(
                        _mm_lddqu_si128((__m128i const *)(data + 32)));
                store_8(UseDelta, UseZigZag, out + 32, data_reg, &prev_reg);
                data_reg = _mm_cvtepu8_epi16(
                        _mm_lddqu_si128((__m128i const *)(data + +40)));
                store_8(UseDelta, UseZigZag, out + 40, data_reg, &prev_reg);
                data_reg = _mm_cvtepu8_epi16(
                        _mm_lddqu_si128((__m128i const *)(data + 48)));
                store_8(UseDelta, UseZigZag, out + 48, data_reg, &prev_reg);
                data_reg = _mm_cvtepu8_epi16(
                        _mm_lddqu_si128((__m128i const *)(data + 56)));
                store_8(UseDelta, UseZigZag, out + 56, data_reg, &prev_reg);
                out += 64;
                data += 64;
                continue;
            }

            data_reg = unpack(keys & 0x00FF, &data);
            store_8(UseDelta, UseZigZag, out, data_reg, &prev_reg);
            data_reg = unpack((keys & 0xFF00) >> 8, &data);
            store_8(UseDelta, UseZigZag, out + 8, data_reg, &prev_reg);

            keys >>= 16;
            data_reg = unpack((keys & 0x00FF), &data);
            store_8(UseDelta, UseZigZag, out + 16, data_reg, &prev_reg);
            data_reg = unpack((keys & 0xFF00) >> 8, &data);
            store_8(UseDelta, UseZigZag, out + 24, data_reg, &prev_reg);

            keys >>= 16;
            data_reg = unpack((keys & 0x00FF), &data);
            store_8(UseDelta, UseZigZag, out + 32, data_reg, &prev_reg);
            data_reg = unpack((keys & 0xFF00) >> 8, &data);
            store_8(UseDelta, UseZigZag, out + 40, data_reg, &prev_reg);

            keys >>= 16;
            data_reg = unpack((keys & 0x00FF), &data);
            store_8(UseDelta, UseZigZag, out + 48, data_reg, &prev_reg);
            data_reg = unpack((keys & 0xFF00) >> 8, &data);
            store_8(UseDelta, UseZigZag, out + 56, data_reg, &prev_reg);

            out += 64;
        }
        {
            uint64_t keys = nextkeys;
            // faster 16-bit delta since we only have 8-bit values
            if (!keys) {  // 64 1-byte ints in a row
                data_reg =
                        _mm_cvtepu8_epi16(_mm_lddqu_si128((__m128i const *)(data)));
                store_8(UseDelta, UseZigZag, out, data_reg, &prev_reg);
                data_reg = _mm_cvtepu8_epi16(
                        _mm_lddqu_si128((__m128i const *)(data + 8)));
                store_8(UseDelta, UseZigZag, out + 8, data_reg, &prev_reg);
                data_reg = _mm_cvtepu8_epi16(
                        _mm_lddqu_si128((__m128i const *)(data + 16)));
                store_8(UseDelta, UseZigZag, out + 16, data_reg, &prev_reg);
                data_reg = _mm_cvtepu8_epi16(
                        _mm_lddqu_si128((__m128i const *)(data + 24)));
                store_8(UseDelta, UseZigZag, out + 24, data_reg, &prev_reg);
                data_reg = _mm_cvtepu8_epi16(
                        _mm_lddqu_si128((__m128i const *)(data + 32)));
                store_8(UseDelta, UseZigZag, out + 32, data_reg, &prev_reg);
                data_reg = _mm_cvtepu8_epi16(
                        _mm_lddqu_si128((__m128i const *)(data + +40)));
                store_8(UseDelta, UseZigZag, out + 40, data_reg, &prev_reg);
                data_reg = _mm_cvtepu8_epi16(
                        _mm_lddqu_si128((__m128i const *)(data + 48)));
                store_8(UseDelta, UseZigZag, out + 48, data_reg, &prev_reg);
                data_reg = _mm_cvtepu8_epi16(
                        _mm_lddqu_si128((__m128i const *)(data + 56)));
                store_8(UseDelta, UseZigZag, out + 56, data_reg, &prev_reg);
                out += 64;
                data += 64;

            } else {
                data_reg = unpack(keys & 0x00FF, &data);
                store_8(UseDelta, UseZigZag, out, data_reg, &prev_reg);
                data_reg = unpack((keys & 0xFF00) >> 8, &data);
                store_8(UseDelta, UseZigZag, out + 8, data_reg, &prev_reg);

                keys >>= 16;
                data_reg = unpack((keys & 0x00FF), &data);
                store_8(UseDelta, UseZigZag, out + 16, data_reg, &prev_reg);
                data_reg = unpack((keys & 0xFF00) >> 8, &data);
                store_8(UseDelta, UseZigZag, out + 24, data_reg, &prev_reg);

                keys >>= 16;
                data_reg = unpack((keys & 0x00FF), &data);
                store_8(UseDelta, UseZigZag, out + 32, data_reg, &prev_reg);
                data_reg = unpack((keys & 0xFF00) >> 8, &data);
                store_8(UseDelta, UseZigZag, out + 40, data_reg, &prev_reg);

                keys >>= 16;
                data_reg = unpack((keys & 0x00FF), &data);
                store_8(UseDelta, UseZigZag, out + 48, data_reg, &prev_reg);
                data_reg = unpack((keys & 0xFF00) >> 8, &data);
                store_8(UseDelta, UseZigZag, out + 56, data_reg, &prev_reg);

                out += 64;
            }
        }
        prev = out[-1];

        keys += key_bytes - (key_bytes & 7);
    }

    return decode_scalar(UseDelta, UseZigZag, out, keys, data, count & 63, prev);
}

#endif  // SVB16_X64
