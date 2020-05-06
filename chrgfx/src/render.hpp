#ifndef CHRGFX__RENDER_H
#define CHRGFX__RENDER_H

#include "bank.hpp"
#include "types.hpp"
#include "utils.hpp"
#include <algorithm>
#include <png++/png.hpp>
#include <stdio.h>

namespace chrgfx
{

struct render_traits {
	u16 cols = 8;
	u8 tile_border = 0;
	bool use_trns = false;
	u8 trns_entry = 0;
};

png::image<png::index_pixel> render(bank &chr_bank, palette &pal,
																		render_traits &rtraits);

} // namespace chrgfx

#endif