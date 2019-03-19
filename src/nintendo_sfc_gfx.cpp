#include "nintendo_sfc_gfx.hpp"

using namespace png;

namespace chrgfx
{
// ----------------- CHR
const chr_traits nintendo_sfc_cx::traits = std_4bpp_tile;

const u8 CHR_PXL_COUNT = 64;	// traits.width * traits.height

const chr_traits* nintendo_sfc_cx::get_traits()
{
	return &nintendo_sfc_cx::traits;
}

const chr* nintendo_sfc_cx::get_chr(const u8* data)
{
	return get_chr_sfc(data);
}

const chr* nintendo_sfc_cx::get_chr_sfc(const u8* data)
{
	auto _out = new chr[CHR_PXL_COUNT];
	u8 this_pxl, pxl_idx{0};

	for(u8 row = 0; row < 16; row += 2)
	{
		for(int8_t shift = 7; shift >= 0; shift--)
		{
			this_pxl = 0;
			if((data[row] >> shift) & 1) this_pxl |= 1;
			if((data[row + 1] >> shift) & 1) this_pxl |= 2;
			if((data[row + 16] >> shift) & 1) this_pxl |= 4;
			if((data[row + 17] >> shift) & 1) this_pxl |= 8;
			_out[pxl_idx++] = this_pxl;
		}
	}

	return _out;
}

const chr_traits nintendo_sfc_3bpp_cx::traits = {
		8,	// 8px width
		8,	// 8px height
		3,	// 3bpp
		24	// 24 bytes
};

const chr_traits* nintendo_sfc_3bpp_cx::get_traits()
{
	return &nintendo_sfc_3bpp_cx::traits;
}

const chr* nintendo_sfc_3bpp_cx::get_chr(const u8* data)
{
	return get_chr_sfc_3bpp(data);
}

const chr* nintendo_sfc_3bpp_cx::get_chr_sfc_3bpp(const u8* data)
{
	auto _out = new chr[CHR_PXL_COUNT];
	u8 this_pxl, pxl_idx{0}, third_plane{16};

	for(u8 row = 0; row < 16; row += 2)
	{
		for(int8_t shift = 7; shift >= 0; shift--)
		{
			this_pxl = 0;
			if((data[row] >> shift) & 1) this_pxl |= 1;
			if((data[row + 1] >> shift) & 1) this_pxl |= 2;
			if((data[third_plane++] >> shift) & 1) this_pxl |= 4;
			_out[pxl_idx++] = this_pxl;
		}
	}

	return _out;
}

const chr_traits nintendo_sfc_8bpp_cx::traits = std_8bpp_tile;

const chr_traits* nintendo_sfc_8bpp_cx::get_traits()
{
	return &nintendo_sfc_8bpp_cx::traits;
}

const chr* nintendo_sfc_8bpp_cx::get_chr(const u8* data)
{
	return get_chr_sfc_8bpp(data);
}

const chr* nintendo_sfc_8bpp_cx::get_chr_sfc_8bpp(const u8* data)
{
	auto _out = new chr[CHR_PXL_COUNT];
	u8 this_pxl, pxl_idx{0};

	for(u8 row = 0; row < 16; row += 2)
	{
		for(int8_t shift = 7; shift >= 0; shift--)
		{
			this_pxl = 0;
			if((data[row] >> shift) & 1) this_pxl |= 1;
			if((data[row + 1] >> shift) & 1) this_pxl |= 2;
			if((data[row + 16] >> shift) & 1) this_pxl |= 4;
			if((data[row + 17] >> shift) & 1) this_pxl |= 8;
			if((data[row + 32] >> shift) & 1) this_pxl |= 0x10;
			if((data[row + 33] >> shift) & 1) this_pxl |= 0x20;
			if((data[row + 48] >> shift) & 1) this_pxl |= 0x40;
			if((data[row + 49] >> shift) & 1) this_pxl |= 0x80;
			_out[pxl_idx++] = this_pxl;
		}
	}

	return _out;
}

// ----------------- PALETTES
// 256 colors per palette, 2 bytes per color
// subpalettes can vary based on graphics mode
// default is 16 subpalettes of 16 colors each
const pal_traits nintendo_sfc_px::traits{256, 2, 16, 16};

const pal_traits* nintendo_sfc_px::get_traits() { return &traits; }

const color* nintendo_sfc_px::get_rgb_sfc(const u8* data)
{
	// 15|               |0
	//   xBBBBBGG GGGRRRRR
	// (little endian)

	u8 r = data[0] & 0x1f;
	u8 g = ((data[0] & 0xe0) >> 5) | ((data[1] & 0x3) << 3);
	u8 b = (data[1] & 0x7c) >> 2;

	r = (r << 3);
	g = (g << 3);
	b = (b << 3);

	return new color(r, g, b);
}

const color* nintendo_sfc_px::get_rgb(const u8* data)
{
	return get_rgb_sfc(data);
}

const palette* nintendo_sfc_px::get_pal(const u8* data, s16 subpal)
{
	return chrgfx::get_pal(this, data, subpal);
}

}	// namespace chrgfx