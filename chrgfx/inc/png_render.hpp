#ifndef CHRGFX__PNG_RENDER_HPP
#define CHRGFX__PNG_RENDER_HPP

#include "blob.hpp"
#include "stdgfx.hpp"
#include "types.hpp"
#include <png++/png.hpp>

namespace chrgfx
{

uint const DEFAULT_ROW_SIZE = 16;
bool const DEFAULT_DRAW_BORDER = false;
bool const DEFAULT_USE_TRNS = false;
uint8_t const DEFAULT_TRNS_INDEX = 0;

/**
 * @brief Tile rendering settings
 */
struct render_config
{
public:
	/**
	 * @brief Number of tiles per row in the output image
	 *
	 */
	uint row_size;

	/**
	 * @brief Draw a 1 pixel border around the inner edges of tiles in the
	 * transparent entry color
	 */
	bool draw_border;

	/**
	 * @brief Enable transparency
	 */
	bool use_trns;

	/**
	 * @brief Palette entry to use for transparency
	 * Also used as the border color when rendering
	 */
	uint8 trns_index;

	render_config() :
			row_size(DEFAULT_ROW_SIZE),
			draw_border(DEFAULT_DRAW_BORDER),
			use_trns(DEFAULT_USE_TRNS),
			trns_index(DEFAULT_TRNS_INDEX)
	{
	}
};

/**
 * @brief Renders a collection of basic (unencoded) tiles to a byte buffer
 */
image render(
	size_t const tile_width, size_t const tile_height, motoi::blob<byte_t> const & chrdata, render_config const & rcfg);

/**
 * @brief Renders a collection of basic (unencoded) tiles to a pixel buffer
 */
/*
chrgfx::pixbuf pixbuf_render(size_t const tile_width,
 size_t const tile_height,
 motoi::blob<std::byte> const & chrdata,
 render_config const & rcfg);
*/

/**
 * @brief Renders a collection of basic (unencoded) tiles to a PNG image
 */
/*
chrgfx::pixbuf png_render(size_t const tile_width,
 size_t const tile_height,
 motoi::blob<std::byte> const & chrdata,
 palette const & pal,
 render_config const & rcfg);
*/

png::image<png::index_pixel> png_render(image const & pixdata, palette const & pal, render_config const & rcfg);
} // namespace chrgfx

#endif