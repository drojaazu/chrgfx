#include "sega_pzlcnst_gfx.hpp"

using namespace png;

namespace chrgfx
{
// 64 colors per palette, 3 bytes per color; 4 subpalettes of 16 colors each
const pal_traits sega_pzlcnst_px::traits = {64, 3, 4, 16};

const pal_traits* sega_pzlcnst_px::get_traits() { return &traits; }

const color* sega_pzlcnst_px::get_rgb_pzl(const u8* data)
{
	// data is similar to Mega Drive format, but split across three bytes
	// R G B
	u8 r = data[0];
	u8 g = data[1];
	u8 b = data[2];

	// slide the three bits up to make things a bit brighter
	r = r << 1;
	g = g << 1;
	b = b << 1;

	// duplicate the data to fill out the whole byte
	r = (r << 4) | r;
	g = (g << 4) | g;
	b = (b << 4) | b;
	return new color(r, g, b);
}

const color* sega_pzlcnst_px::get_rgb(const u8* data)
{
	return get_rgb_pzl(data);
}
}	// namespace chrgfx