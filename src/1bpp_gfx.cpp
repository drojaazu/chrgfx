#include "1bpp_gfx.hpp"

using namespace png;

namespace gfx
{
const chr_traits bpp1_cx::traits = gfx::std_1bpp_tile;

const chr_traits* bpp1_cx::get_traits() { return &bpp1_cx::traits; }

const chr* bpp1_cx::get_chr_1bpp(BYTE* data)
{
	// for 8x8 1bpp chr, 1 row = 1 byte
	auto _out = new chr(traits.width, traits.height);
	auto this_pxlrow = std::vector<index_pixel>();
	for(size_t row = 0; row < traits.height; row++)
	{
		this_pxlrow.clear();
		this_pxlrow.reserve(traits.width);
		for(int8_t shift = 7; shift >= 0; shift--)
		{
			if(data[row] >> shift & 1)
				this_pxlrow.push_back(1);
			else
				this_pxlrow.push_back(0);
		}

		_out->put_row(row, this_pxlrow);
	}
	return _out;
}

const chr* bpp1_cx::get_chr(BYTE* data) { return bpp1_cx::get_chr_1bpp(data); }

}	// namespace gfx
