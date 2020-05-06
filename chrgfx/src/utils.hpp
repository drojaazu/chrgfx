#ifndef CHRGFX__UTILS_H
#define CHRGFX__UTILS_H

#include "types.hpp"
#include <algorithm>

using png::color;
using png::palette;

namespace chrgfx
{
u8 expand_bits(u8 data, u8 bitcount);

u32 create_bitmask32(u8 bitcount);

u16 create_bitmask16(u8 bitcount);

u8 create_bitmask8(u8 bitcount);

png::palette make_pal(bool blank);

palette make_pal_random();

void fill_pal(palette *pal);

bool is_system_bigendian();

const static bool bigend_sys = is_system_bigendian();
} // namespace chrgfx

#endif