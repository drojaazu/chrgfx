#ifndef RENDER_H
#define RENDER_H

#include "bank.hpp"
#include "gfxdef.hpp"
#include <png++/png.hpp>

namespace chrgfx
{
struct render_traits {
	u16 cols = 8;
	u8 tile_border = 0;
	bool use_trns = false;
	u8 trns_entry = 0;
};

png::image<png::index_pixel> render(bank &chr_bank, png::palette &pal,
																		render_traits &traits);
} // namespace chrgfx

#endif
