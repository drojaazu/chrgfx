#ifndef GFX_UTILS_H
#define GFX_UTILS_H

#include "gfx_types.hpp"

using namespace png;

namespace chrgfx
{
u32 create_bitmask32(u8 bitcount);
u16 create_bitmask16(u8 bitcount);
u8 create_bitmask8(u8 bitcount);
u8 expand_bits(u8 data, u8 bitcount);

// helper functions
// bank* get_bank(chr_xform* xform, u8* data, size_t count);
palette* make_pal(bool blank = false);

void fill_pal(palette* pal);

}	// namespace chrgfx

#endif