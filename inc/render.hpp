#ifndef RENDER_H
#define RENDER_H

#include "utils.hpp"

using namespace gfx;
using namespace png;

struct render_traits
{
	WORD cols = 8;
	BYTE tile_border = 0;
	BYTE palette_offset = 0;
	bool use_trns = false;
	BYTE trns_entry = 0;
};

image<index_pixel>* render(bank* chr_bank, const palette* pal,
													 render_traits* traits);

#endif
