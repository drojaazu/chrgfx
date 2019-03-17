#include "sega_md_gfx.hpp"

using namespace png;

namespace chrgfx
{
// ----------------- CHR
const chr_traits sega_md_cx::traits = std_4bpp_tile;

const u8 CHR_PXL_COUNT = 64;	// traits.width * traits.height

const chr_traits* sega_md_cx::get_traits() { return &sega_md_cx::traits; }

const chr* sega_md_cx::get_chr_smd(chr* data)
{
	u8 data_idx{0}, pxl_iter;

	auto outchr = new chr[CHR_PXL_COUNT];

	for(pxl_iter = 0; pxl_iter < CHR_PXL_COUNT;)
	{
		outchr[pxl_iter++] = ((data[data_idx] & 0xf0) >> 4);
		outchr[pxl_iter++] = (u8)(data[data_idx++] & 0xf);
	}

	return outchr;
}

// ----------------- PALETTES
// 64 colors per palette, 2 bytes per color; 4 subpalettes of 16 colors each
const pal_traits sega_md_px::traits = {64, 2, 4, 16};

const pal_traits* sega_md_px::get_traits() { return &traits; }

const chr* sega_md_cx::get_chr(chr* data) { return get_chr_smd(data); }

// PALETTES
const color* sega_md_px::get_rgb_smd(u8* data)
{
	// 16|               |0
	//   xxxxBBBB GGGGRRRR

	// duplicate the four bits to fill out the whole byte
	u8 b = data[0] & 0xf;
	b = (b << 4) | b;
	u8 g = (data[1] & 0xf0);
	g = (g >> 4) | g;
	u8 r = data[1] & 0xf;
	r = (r << 4) | r;

	return new color(r, g, b);
}

const color* sega_md_px::get_rgb(u8* data) { return get_rgb_smd(data); }

const palette* sega_md_px::get_pal(u8* data, int8_t subpal)
{
	return chrgfx::get_pal(this, data, subpal);
}

}	// namespace chrgfx