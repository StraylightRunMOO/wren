#ifndef pigeon_math_h
#define pigeon_math_h

#include <math.h>
#include <stdint.h>

// A union to let us reinterpret a double as raw bits and back.
typedef union
{
  uint64_t bits64;
  uint32_t bits32[2];
  double num;
} PigeonDoubleBits;

#define PIGEON_DOUBLE_QNAN_POS_MIN_BITS (UINT64_C(0x7FF8000000000000))
#define PIGEON_DOUBLE_QNAN_POS_MAX_BITS (UINT64_C(0x7FFFFFFFFFFFFFFF))

#define PIGEON_DOUBLE_NAN (pigeonDoubleFromBits(PIGEON_DOUBLE_QNAN_POS_MIN_BITS))

static inline double pigeonDoubleFromBits(uint64_t bits)
{
  PigeonDoubleBits data;
  data.bits64 = bits;
  return data.num;
}

static inline uint64_t pigeonDoubleToBits(double num)
{
  PigeonDoubleBits data;
  data.num = num;
  return data.bits64;
}

#endif
