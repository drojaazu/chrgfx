#include "sega_md_gfx.hpp"

using namespace png;

namespace chrgfx
{
// ----------------- CHR
const chr_traits sega_md_cx::traits = std_4bpp_tile;

const u8 CHR_PXL_COUNT = 64;	// traits.width * traits.height

const chr_traits* sega_md_cx::get_traits() { return &sega_md_cx::traits; }

const chr* sega_md_cx::get_chr_smd(const chr* data)
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

const chr* sega_md_cx::get_chr(const chr* data) { return get_chr_smd(data); }

// PALETTES
const color* sega_md_px::get_rgb_smd(const u8* data)
{
	// 15|               |0
	//   xxxxBBBx GGGxRRRx
	u8 g = (data[1] & 0xe0);
	u8 b = data[0] & 0xe;
	u8 r = data[1] & 0xe;

	// duplicate the data to fill out the whole byte
	r = (r << 4) | r;
	g = (g >> 4) | g;
	b = (b << 4) | b;
	return new color(r, g, b);
}

const color* sega_md_px::get_rgb(const u8* data) { return get_rgb_smd(data); }

const palette* sega_md_px::get_pal(const u8* data, s16 subpal)
{
	return chrgfx::get_pal(this, data, subpal);
}

}	// namespace chrgfx