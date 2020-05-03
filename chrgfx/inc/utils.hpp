#ifndef GFX_UTILS_H
#define GFX_UTILS_H

#include "types.hpp"

using namespace png;

namespace chrgfx
{
u32 create_bitmask32(u8 bitcount);
u16 create_bitmask16(u8 bitcount);
u8 create_bitmask8(u8 bitcount);

u8 expand_bits(u8 data, u8 bitcount);

palette make_pal(bool blank = false);

// void fill_pal(palette *pal);

bool is_system_bigendian();

const static bool bigend_sys = is_system_bigendian();

} // namespace chrgfx

#endif