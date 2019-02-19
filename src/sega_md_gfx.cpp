#include "sega_md_gfx.hpp"

using namespace png;

namespace gfx
{
// TILES
const chr_traits sega_md_cx::traits = std_4bpp_tile;

const chr_traits* sega_md_cx::get_traits() { return &sega_md_cx::traits; }

const chr* sega_md_cx::get_chr_smd(u8* data)
{
	chr* outchr = new chr(traits.width, traits.height);

	u8 dataIter{0}, rowIter, pixelIter;
	std::vector<index_pixel> thisRow = std::vector<index_pixel>();

	for(rowIter = 0; rowIter < traits.height; rowIter++)
	{
		thisRow.clear();
		thisRow.reserve(traits.width);
		for(pixelIter = 0; pixelIter < 4; pixelIter++)
		{
			thisRow.push_back((data[dataIter] & 0xf0) >> 4);
			thisRow.push_back(data[dataIter++] & 0xf);
		}
		outchr->put_row(rowIter, thisRow);
	}

	return outchr;
}

// ------- PALETTES

// 64 colors per palette, 2 bytes per color
const pal_traits sega_md_px::traits = {64, 2};
const pal_traits* sega_md_px::get_traits() { return &sega_md_px::traits; }

const chr* sega_md_cx::get_chr(u8* data) { return get_chr_smd(data); }

// PALETTES
const color* sega_md_px::get_rgb_smd(u8* data)
{
	//  xxxxBBBB GGGGRRRR, linear

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

const palette* sega_md_px::get_pal(u8* data)
{
	return gfx::get_pal(this, data, 64);
}

}	// namespace gfx