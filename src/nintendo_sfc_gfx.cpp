#include "nintendo_sfc_gfx.hpp"

using namespace png;

namespace gfx
{
// -------- CHR
const chr_traits nintendo_sfc_cx::traits = std_4bpp_tile;

const chr_traits* nintendo_sfc_cx::get_traits()
{
	return &nintendo_sfc_cx::traits;
}

const chr* nintendo_sfc_cx::get_chr(BYTE* data) { return get_chr_sfc(data); }

const chr* nintendo_sfc_cx::get_chr_sfc(BYTE* data)
{
	auto _out = new chr(traits.width, traits.height);
	size_t pxlRowCount = 0;
	BYTE thisPxl;
	auto thisPxlRow = std::vector<index_pixel>();

	for(size_t row = 0; row < 16; row += 2)
	{
		thisPxlRow.clear();
		thisPxlRow.reserve(8);
		for(int8_t shift = 7; shift >= 0; shift--)
		{
			thisPxl = 0;
			if((data[row] >> shift) & 1) thisPxl++;
			if((data[row + 1] >> shift) & 1) thisPxl += 2;
			if((data[row + 16] >> shift) & 1) thisPxl += 4;
			if((data[row + 17] >> shift) & 1) thisPxl += 8;
			thisPxlRow.push_back(thisPxl);
		}
		_out->put_row(pxlRowCount, thisPxlRow);
		pxlRowCount++;
	}

	return _out;
}

// ----------------- PALETTE
// 256 colors per palette, 2 bytes per color
const pal_traits nintendo_sfc_px::traits = {255, 2};

const pal_traits* nintendo_sfc_px::get_traits()
{
	return &nintendo_sfc_px::traits;
}

const color* nintendo_sfc_px::get_rgb(BYTE* data)
{
	// 0BBBBBGGGGGRRRRR, planar, little endian
	// GGGRRRRR0BBBBBGG
	// |||G lo bits  ||G hi bits

	BYTE r = data[0] & 0x1f;
	BYTE g = ((data[0] & 0xe0) >> 5) | ((data[1] & 0x3) << 3);
	BYTE b = (data[1] & 0x7c) >> 2;

	// stretch the color range so it works with rgb
	r = (r * 255) / 31;
	g = (g * 255) / 31;
	b = (b * 255) / 31;

	return new color(r, g, b);
}

const palette* nintendo_sfc_px::get_pal(BYTE* data)
{
	return gfx::get_pal(this, data, 256);
}

}	// namespace gfx