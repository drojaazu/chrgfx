#ifndef CHRGFX__RENDER_H
#define CHRGFX__RENDER_H

#include "types.hpp"
#include "utils.hpp"
#include <algorithm>
#include <png++/png.hpp>
#include <stdio.h>

class buffer;

namespace chrgfx
{

	/**
	 * Options for rendering a PNG image from a collection of tiles
	 */
	struct render_traits
	{
		/**
		 * Number of columns of tiles in the PNG image
		 */
		u16 cols = 8;

		/**
		 * Draws a 1 pixel border around the inner edges of tiles in the transparent
		 * entry color
		 */
		bool draw_border = false;

		/**
		 * Enable transparency
		 */
		bool use_trns = false;

		/**
		 * Palette entry to use for transparency
		 */
		u8 trns_entry = 0;
	};

	/**
	 * Renders the specified bank of tiles to a pixel buffer
	 * Does not perform data conversion
	 * Tiles should already be converted to standard format (24bit packed pixel)
	 */
	png::pixel_buffer<png::index_pixel> render(chrdef const & chrdef,
																						 vector<buffer> & chrdata,
																						 render_traits const & rtraits);

	/**
	 * Renders the specifed bank of tiles and color palette to a PNG image
	 * Does not perform data conversion
	 * Tiles should already be converted to standard format (24bit packed pixel)
	 */
	png::image<png::index_pixel> png_render(chrdef const & chrdef,
																					vector<buffer> & chrdata,
																					palette const & pal,
																					render_traits const & rtraits);

} // namespace chrgfx

#endif