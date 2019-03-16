#include "snk_ngp_gfx.hpp"

using namespace png;

namespace chrgfx
{
// ----------------- CHR
const chr_traits snk_ngp_cx::traits = std_2bpp_tile;

const u8 CHR_PXL_COUNT = 64;

const chr_traits* snk_ngp_cx::get_traits() { return &snk_ngp_cx::traits; }

const chr* snk_ngp_cx::get_chr(u8* data) { return get_chr_ngp(data); }

const chr* snk_ngp_cx::get_chr_ngp(u8* data)
{
	auto _out = new chr[CHR_PXL_COUNT];
	u8 pxl_idx{0};

	for(u8 row = 1; row < 17; row += 2)
	{
		for(int8_t shift = 6; shift >= 0; shift -= 2)
		{
			_out[pxl_idx++] = (data[row] >> shift) & 3;
		}
		row--;
		for(int8_t shift = 6; shift >= 0; shift -= 2)
		{
			_out[pxl_idx++] = (data[row] >> shift) & 3;
		}
		row++;
	}

	return _out;
}

// ----------------- PALETTES

// NeoGeo Pocket (monochrome)
// 6 palettes, 4 colors each; 1 byte per color
const pal_traits snk_ngp_px::traits = {8, 2};

const pal_traits* snk_ngp_px::get_traits() { return &snk_ngp_px::traits; }

// colors based on MAME output
const color snk_ngp_px::ngp_colors[] = {
		color(0xff, 0xff, 0xff),	// 00 - lightest
		color(0xdb, 0xdb, 0xdb),	// 01
		color(0xb6, 0xb6, 0xb6),	// 02
		color(0x92, 0x92, 0x92),	// 03
		color(0x6d, 0x6d, 0x6d),	// 04
		color(0x49, 0x49, 0x49),	// 05
		color(0x24, 0x24, 0x24),	// 06
		color(0, 0, 0)};					// 07 - darkest

const color* snk_ngp_px::get_rgb(u8* data)
{
	if(*data > 3) throw std::out_of_range("Invalid NeoGeo Pocket palette entry");
	return &ngp_colors[*data];
}

const palette* snk_ngp_px::get_pal(u8* data)
{
	auto _out = new palette();
	_out->reserve(24);

	// bits 0-2 = 8 possible shades
	for(u8 this_color = 0; this_color < 24; this_color++)
	{
		_out->push_back(ngp_colors[data[this_color] & 3]);
	}

	return _out;
}

// NeoGeo Pocket Color
// 16 palettes, 4 colors each; 2 bytes per color
const pal_traits snk_ngpc_px::traits = {64, 2};

const pal_traits* snk_ngpc_px::get_traits() { return &snk_ngpc_px::traits; }

const color* snk_ngpc_px::get_rgb(u8* data)
{
	// 16|               |0
	//   GGGGBBBB xxxxRRRR
	// little endian

	u8 b = data[1] & 0xf;
	b = (b << 4) | b;
	u8 g = (data[0] & 0xf0);
	g = (g >> 4) | g;
	u8 r = data[0] & 0xf;
	r = (r << 4) | r;

	return new color(r, g, b);
}

const palette* snk_ngpc_px::get_pal(u8* data)
{
	return chrgfx::get_pal(this, data, 64);
}

}	// namespace chrgfx