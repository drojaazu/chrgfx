#ifndef GFX_UTILS_H
#define GFX_UTILS_H

#include "gfx_types.hpp"

using namespace png;

namespace chrgfx
{
u32 create_bitmask(u8 bitcount);

const chr_traits std_8bpp_tile{
		8,	// 8px width
		8,	// 8px height
		8,	// 8bpp
		64	// 64 bytes
};

const chr_traits std_4bpp_tile{
		8,	// 8px width
		8,	// 8px height
		4,	// 4bpp
		32	// 32 bytes
};

const chr_traits std_2bpp_tile{
		8,	// 8px width
		8,	// 8px height
		2,	// 2bpp
		16	// 16 bytes
};

const chr_traits std_1bpp_tile{
		8,	// 8px width
		8,	// 8px height
		1,	// 1bpp
		8		// 8 bytes
};

// helper functions
// bank* get_bank(chr_xform* xform, u8* data, size_t count);
palette* make_pal(bool blank = false);
palette* get_pal(pal_xform* xform, const u8* data, s16 subpal);
void fill_pal(palette* pal);

}	// namespace chrgfx

#endif