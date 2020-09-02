#ifndef CHRGFX__RENDER_H
#define CHRGFX__RENDER_H

#include "chrbank.hpp"
#include "types.hpp"
#include "utils.hpp"
#include <algorithm>
#include <png++/png.hpp>
#include <stdio.h>

namespace chrgfx
{

/**
 * Options for rendering a PNG image from a collection of tiles
 */
struct render_traits {
	/**
	 * Number of columns of tiles in the PNG image
	 */
	u16 cols = 8;

	/**
	 * Enable transparency
	 */
	bool use_trns = false;

	/**
	 * Specify palette entry for transparency
	 */
	u8 trns_entry = 0;
};

/**
 * Renders the specifed bank of tiles and color palette to a PNG image
 */
png::image<png::index_pixel> png_render(chrbank const &chr_bank,
																				palette const &pal,
																				render_traits const &rtraits);

} // namespace chrgfx

#endif