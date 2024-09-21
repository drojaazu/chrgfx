#include "basic_gfx.hpp"
#include "chrdef.hpp"
#include <optional>

#ifndef __CHRGFX__TILESETCONV_HPP
#define __CHRGFX__TILESETCONV_HPP

namespace chrgfx
{

uint const DEFAULT_ROW_SIZE = 16;
bool const DEFAULT_USE_TRNS = false;

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
	uint row_size {DEFAULT_ROW_SIZE};

	/**
	 * @brief Palette entry to use for transparency
	 * Also used as the border color when rendering
	 */
	std::optional<uint8> trns_index {std::nullopt};
};

/**
 * @brief Renders a collection of tiles to a basic image
 */
basic_image render_chrset(
	chrdef const & chrdef, byte_t const * chrset_data, size_t chrset_datasize, render_config const & rcfg);

/**
 * @brief Returns a collection of basic tiles segmented from a given image
 */
size_t make_chrset(byte_t ** out, chrdef const & chrdef, basic_image const & bitmap);

} // namespace chrgfx

#endif
