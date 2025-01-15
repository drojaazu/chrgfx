#ifndef CHRGFX__BUILTIN_DEFS_H
#define CHRGFX__BUILTIN_DEFS_H

#include "builtin_defs.hpp"
#include "chrdef.hpp"
#include "coldef.hpp"
#include "paldef.hpp"
#include "utils.hpp"
#ifdef DEBUG
// we must include iostream here as cerr is used for debugging when initializing some static variables
#include <iostream>
#endif

namespace chrgfx::gfxdefs
{

using namespace std;

// clang-format off
/**
 * Minimal, generic CHR format
 * 1bpp 8x8
 */
chrdef const chr_8x8_1bpp {"chr_8x8_1bpp", 8, 8, 1, {0}, {0, 1, 2, 3, 4, 5, 6, 7}, {0, 8, 16, 24, 32, 40, 48, 56}};

chrdef const chr_8x8_2bpp_packed_lsb {
	"chr_8x8_2bpp_packed_lsb",
	8,
	8,
	2,
	{STEP8(0, 2)},
	{STEP8(0, 2 * 8)},
	{STEP2(1, -1)},
	"Size: 8x8, Depth: 2 bit, Layout: Packed (LSB)"
};


chrdef const chr_8x8_2bpp_packed_msb {
	"chr_8x8_2bpp_packed_msb",
	8,
	8,
	2,
	{STEP8(0, 2)},
	{STEP8(0, 2 * 8)},
	{STEP2(0, 1)},
	"Size: 8x8, Depth: 2 bit, Layout: Packed (MSB)"
};

chrdef const chr_8x8_4bpp_packed_lsb {
	"chr_8x8_4bpp_packed_lsb",
	8,
	8,
	4,
	{STEP8(0, 4)},
	{STEP8(0, 4 * 8)},
	{STEP4(3, -1)},
	"Size: 8x8, Depth: 4 bit, Layout: Packed (LSB)"
};

chrdef const chr_8x8_4bpp_packed_msb {
	"chr_8x8_4bpp_packed_msb",
	8,
	8,
	4,
	{STEP8(0, 4)},
	{STEP8(0, 4 * 8)},
	{STEP4(0, 1)},
	"Size: 8x8, Depth: 4 bit, Layout: Packed (MSB)"
};

chrdef const chr_8x8_8bpp_packed_lsb {
	"chr_8x8_8bpp_packed_lsb",
	8,
	8,
	8,
	{STEP8(0, 8)},
	{STEP8(0, 8 * 8)},
	{STEP8(7, -1)},
	"Size: 8x8, Depth: 8 bit, Layout: Packed (LSB)"
};

chrdef const chr_8x8_8bpp_packed_msb {
	"chr_8x8_8bpp_packed_msb",
	8,
	8,
	8,
	{STEP8(0, 8)},
	{STEP8(0, 8 * 8)},
	{STEP8(0, 1)},
	"Size: 8x8, Depth: 8 bit, Layout: Packed (MSB)"
};

chrdef const chr_8x8_2bpp_planar {
	"chr_8x8_2bpp_planar",
	8,
	8,
	2,
	{STEP8(0, 1)},
	{STEP8(0, 2 * 8)},
	{STEP2(0, 8)},
	"Size: 8x8, Depth: 2 bit, Layout: Planar"
};

chrdef const chr_8x8_4bpp_planar {
	"chr_8x8_4bpp_planar",
	8,
	8,
	4,
	{STEP8(0, 1)},
	{STEP8(0, 4 * 8)},
	{STEP4(0, 8)},
	"Size: 8x8, Depth: 4 bit, Layout: Planar"
};

map<string, chrdef const &> const chrdefs {
	{chr_8x8_2bpp_packed_lsb.id(), chr_8x8_2bpp_packed_lsb},
	{chr_8x8_2bpp_packed_msb.id(), chr_8x8_2bpp_packed_msb},
	{chr_8x8_4bpp_packed_msb.id(), chr_8x8_4bpp_packed_msb},
	{chr_8x8_4bpp_packed_lsb.id(), chr_8x8_4bpp_packed_lsb},
	{chr_8x8_8bpp_packed_msb.id(), chr_8x8_8bpp_packed_msb},
	{chr_8x8_8bpp_packed_lsb.id(), chr_8x8_8bpp_packed_lsb},
	{chr_8x8_2bpp_planar.id(), chr_8x8_2bpp_planar},
	{chr_8x8_4bpp_planar.id(), chr_8x8_4bpp_planar}
};
// clang-format on

// clang-format off
/**
 * @brief BGR 222 color format
 * @details Data is aligned to LSB; upper bits (MSB) are unused
 * 
 */
rgbcoldef const col_bgr_222_packed {
	"col_bgr_222_packed",
	2,
	{
		{{0, 2}, {2, 2}, {4, 2}}
	},
	false,
	"BGR 222 (LSB)"
};

/**
 * @brief BGR 444 color format
 * @details Data is aligned to LSB; upper bits (MSB) are unused
 * 
 */
rgbcoldef const col_bgr_333_packed {
	"col_bgr_333_packed",
	3,
	{
		{{0, 3}, {3, 3}, {3, 3}}
	},
	false,
	"BGR 333 (LSB)"
};

/**
 * @brief BGR 444 color format
 * @details Data is aligned to LSB; upper bits (MSB) are unused
 * 
 */
rgbcoldef const col_bgr_444_packed {
	"col_bgr_444_packed",
	4,
	{
		{{0, 4}, {4, 4}, {8, 4}}
	},
	false,
	"BGR 444 (LSB)"
};

/**
 * @brief BGR 555 color format
 * @details Data is aligned to LSB; upper bits (MSB) are unused
 * 
 */
rgbcoldef const col_bgr_555_packed {
	"col_bgr_555_packed",
	5,
	{
		{{0, 5}, {5, 5}, {10, 5}}
	},
	false,
	"BGR 555 (LSB)"
};

map<string, rgbcoldef const &> const rgbcoldefs {
	{col_bgr_222_packed.id(), col_bgr_222_packed},
	{col_bgr_333_packed.id(), col_bgr_333_packed},
	{col_bgr_444_packed.id(), col_bgr_444_packed},
	{col_bgr_555_packed.id(), col_bgr_555_packed}
};

// clang-format on

// clang-format off
paldef const pal_16bit_8color {
	"pal_16bit_8color",
	16,
	8
};

paldef const pal_16bit_16color {
	"pal_16bit_16color",
	16,
	16
};

paldef const pal_16bit_32color {
	"pal_16bit_32color",
	16,
	32
};

paldef const pal_16bit_64color {
	"pal_16bit_64color",
	16,
	64
};

paldef const pal_16bit_256color {
	"pal_16bit_256color",
	16,
	256
};

map<string, paldef const &> const paldefs {
	{pal_16bit_8color.id(), pal_16bit_8color},
	{pal_16bit_16color.id(), pal_16bit_16color},
	{pal_16bit_32color.id(), pal_16bit_32color},
	{pal_16bit_64color.id(), pal_16bit_64color},
	{pal_16bit_256color.id(), pal_16bit_256color}
};

// clang-format on
} // namespace chrgfx::gfxdefs

#endif