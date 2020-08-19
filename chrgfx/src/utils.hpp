#ifndef CHRGFX__UTILS_H
#define CHRGFX__UTILS_H

#include "types.hpp"
#include <algorithm>
#include <png++/png.hpp>

using png::color;
using png::palette;

namespace chrgfx
{
u8 dither_bits(u8 data, u8 bitcount);

u8 expand_bits(u8 data, u8 bitcount);

u32 create_bitmask32(u8 bitcount);

u16 create_bitmask16(u8 bitcount);

u8 create_bitmask8(u8 bitcount);

palette make_pal_random();

bool is_system_bigendian();

extern bool const bigend_sys;
} // namespace chrgfx

#endif