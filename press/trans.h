#ifndef TRANS_H
#define TRANS_H

#include <stdint.h>

uint16_t *shift_x_u16(int16_t x, const uint16_t *in, uint64_t nin);
int16_t *shift_x_16(int16_t x, const int16_t *in, uint64_t nin);
void shift_x_u16_u16(int16_t x, const uint16_t *in, uint64_t nin,
		     uint16_t *out);
void shift_x_inplace_u16(int16_t x, uint16_t *in, uint64_t nin);
void shift_x_inplace_16(int16_t x, int16_t *in, uint64_t nin);
void shift_x_inplace_u8(int8_t x, uint8_t *in, uint64_t nin);
void zigzag_inplace_8(int8_t *in, uint64_t nin);
void zigzag_inplace_16(int16_t *in, uint64_t nin);
void unzigzag_inplace_16(uint16_t *in, uint64_t nin);
int16_t *delta_16(const int16_t *in, uint64_t nin);
uint32_t *delta_increasing_u32(const uint32_t *in, uint64_t nin);
uint16_t *delta_increasing_u32_u16(const uint32_t *in, uint64_t nin);
void undelta_inplace_16(int16_t *in, uint64_t nin);
void undelta_inplace_increasing_u32(uint32_t *in, uint64_t nin);
uint16_t *zigdelta_16(const int16_t *in, uint64_t nin);
uint16_t *zigdelta_16_u16(const int16_t *in, uint64_t nin);
uint32_t *zigdelta_16_u32(const int16_t *in, uint64_t nin);
void unzigdelta_inplace_16(int16_t *in, uint64_t nin);
void unzigdelta_u16_16(const uint16_t *in, uint64_t nin, int16_t *out);
void unzigdelta_u32_16(const uint32_t *in, uint64_t nin, int16_t *out);
void times_x_inplace_16(int16_t x, int16_t *in, uint64_t nin);

#endif /* trans.h */
