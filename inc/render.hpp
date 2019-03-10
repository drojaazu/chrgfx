#ifndef RENDER_H
#define RENDER_H

#include "utils.hpp"

using namespace chrgfx;
using namespace png;

struct render_traits
{
	u16 cols = 8;
	u8 tile_border = 0;
	u8 palette_offset = 0;
	bool use_trns = false;
	u8 trns_entry = 0;
};

image<index_pixel>* render(bank* chr_bank, const palette* pal,
													 render_traits* traits);

#endif
