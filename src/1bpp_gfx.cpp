#include "1bpp_gfx.hpp"

using namespace png;

namespace chrgfx
{
const chr_traits bpp1_cx::traits = std_1bpp_tile;

const u8 CHR_PXL_COUNT = 64;	// traits.width * traits.height

const chr_traits* bpp1_cx::get_traits() { return &bpp1_cx::traits; }

const chr* bpp1_cx::get_chr_1bpp(const u8* data)
{
	// for 8x8 1bpp chr, 1 row = 1 u8
	u8 pxl_idx{0};

	auto _out = new chr[CHR_PXL_COUNT];
	for(u8 row = 0; row < traits.height; row++)
	{
		for(int8_t shift = 7; shift >= 0; shift--)
		{
			if(data[row] >> shift & 1)
				_out[pxl_idx] = 1;
			else
				_out[pxl_idx] = 0;
		}
	}
	return _out;
}

const chr* bpp1_cx::get_chr(const u8* data)
{
	return bpp1_cx::get_chr_1bpp(data);
}

}	// namespace chrgfx
