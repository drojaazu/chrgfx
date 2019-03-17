#include "sega_8bit_gfx.hpp"

using namespace png;

namespace chrgfx
{
// ----------------- CHR
const chr_traits sega_8bit_cx::traits = std_4bpp_tile;

const u8 CHR_PXL_COUNT = 64;	// traits.width * traits.height

const chr_traits* sega_8bit_cx::get_traits() { return &sega_8bit_cx::traits; }

const chr* sega_8bit_cx::get_chr(u8* data) { return get_chr_sega8bit(data); }

const chr* sega_8bit_cx::get_chr_sega8bit(u8* data)
{
	auto _out = new chr[CHR_PXL_COUNT];
	u8 this_pxl, pxl_idx{0};

	for(u8 row = 0; row < 32; row += 4)
	{
		for(int8_t shift = 7; shift >= 0; shift--)
		{
			this_pxl = 0;
			if((data[row] >> shift) & 1) this_pxl += 1;
			if((data[row + 1] >> shift) & 1) this_pxl += 2;
			if((data[row + 2] >> shift) & 1) this_pxl += 4;
			if((data[row + 3] >> shift) & 1) this_pxl += 8;
			_out[pxl_idx++] = this_pxl;
		}
	}

	return _out;
}

// ----------------- PALETTES
// ------ Mark III/Master System
// 32 colors per palette, 1 byte per color; 2 subpalettes, 16 colors each
// subpalette 0 - background palette, 1 - sprite palette
// http://www.smspower.org/maxim/HowToProgram/Palette
const pal_traits sega_mastersys_px::traits{32, 1, 2, 16};

const pal_traits* sega_mastersys_px::get_traits() { return &traits; }

const color* sega_mastersys_px::get_rgb_ms(u8* data)
{
	// 8|      |0
	//  xxBBGGRR

	u8 r = data[0] & 3;
	u8 g = (data[0] & 0xC) >> 2;
	u8 b = (data[0] & 0x30) >> 4;
	r = (((r << 2) | r) << 4) | (r << 2 | r);
	g = (((g << 2) | g) << 4) | (g << 2 | g);
	b = (((b << 2) | b) << 4) | (b << 2 | b);

	return new color(r, g, b);
}

const color* sega_mastersys_px::get_rgb(u8* data) { return get_rgb_ms(data); }

const palette* sega_mastersys_px::get_pal(u8* data, int8_t subpalette)
{
	return chrgfx::get_pal(this, data, subpalette);
}

// ------ GameGear
// 32 colors per palette, 2 byte per color; 2 subpalettes, 16 colors each
const pal_traits sega_gamegear_px::traits{32, 2, 2, 16};

const pal_traits* sega_gamegear_px::get_traits() { return &traits; }

const color* sega_gamegear_px::get_rgb_gg(u8* data)
{
	// 16|               |0
	//   xxxxBBBB GGGGRRRR
	// (little endian)

	u8 g = (data[0] & 0xf0) >> 4;
	u8 b = data[1] & 0x0f;
	u8 r = data[0] & 0x0f;

	r = (r << 4) | r;
	g = (g << 4) | g;
	b = (b << 4) | b;

	return new color(r, g, b);
}

const color* sega_gamegear_px::get_rgb(u8* data) { return get_rgb_gg(data); }

const palette* sega_gamegear_px::get_pal(u8* data, int8_t subpalette)
{
	return chrgfx::get_pal(this, data, subpalette);
}

}	// namespace chrgfx