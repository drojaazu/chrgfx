#ifndef CHRGFX__PNG_RENDER_HPP
#define CHRGFX__PNG_RENDER_HPP

#include "buffer.hpp"
#include "chrdef.hpp"
#include "types.hpp"
#include "utils.hpp"
#include <png++/png.hpp>

namespace chrgfx
{
using namespace png;

/**
 * @brief PNG render settings
 */
struct render_traits
{
public:
	/**
	 * @brief Number of columns in a row in the output image
	 *
	 */
	ushort columns;

	/**
	 * Draw a 1 pixel border around the inner edges of tiles in the transparent
	 * entry color
	 */
	bool draw_border;

	/**
	 * Enable transparency
	 */
	bool use_trns;

	/**
	 * Palette entry to use for transparency
	 */
	u8 trns_index;
};

/**
 * @brief Renders a collection of basic (unencoded) tiles to a pixel buffer
 */
png::pixel_buffer<png::index_pixel> render(chrdef const & chrdef,
																					 buffer<byte_t> const & chrdata,
																					 render_traits const & rtraits);

/**
 * @brief Renders a collection of basic (unencoded) tiles to a PNG image
 */
png::image<png::index_pixel> png_render(chrdef const & chrdef,
																				buffer<byte_t> const & chrdata,
																				palette const & pal,
																				render_traits const & rtraits);

} // namespace chrgfx

#endif