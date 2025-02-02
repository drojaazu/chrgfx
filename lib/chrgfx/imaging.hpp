/**
 * @file imaging.hpp
 * @author Damian Rogers / damian@motoi.pro
 * @copyright ©2022 Motoi Productions / Released under MIT License
 * @brief Tileset conversion functions
 */

#include "chrdef.hpp"
#include "coldef.hpp"
#include "image_types.hpp"
#include "palconv.hpp"
#include "paldef.hpp"
#include <optional>

#ifndef __CHRGFX__IMAGING_HPP
#define __CHRGFX__IMAGING_HPP

namespace chrgfx
{

uint const DEFAULT_ROW_SIZE = 16;

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
	 *
	 */
	std::optional<uint8> trns_index {std::nullopt};
};

/**
 * @brief Renders a collection of basic tiles (tileset) to a bitmap image
 *
 * @param chrdef Tile encoding definition
 * @param in_chrset Pointer to input basic tileset
 * @param in_chrset_datasize Size of input basic tilset in bytes
 * @param render_cfg Tileset rendering options
 */
image render_tileset(
	chrdef const & chrdef, byte_t const * in_chrset, size_t const in_chrset_datasize, render_config const & render_cfg);

/**
 * @brief Renders a palette as color swatches in an indexed bitmap image
 *
 * @param paldef Palette encoding definition
 * @param coldef Color encoding definition
 * @param in_palette Pointer to input encoded palette
 */
image render_palette(paldef const & paldef, coldef const & coldef, byte_t const * in_palette);

/**
 * @brief Renders an arbitrary collection of palette lines as color swatches in a direct color bitmap image
 *
 * @param paldef Palette encoding definition
 * @param coldef Color encoding definition
 * @param in_palette Pointer to input encoded palette
 */
motoi::image<rgb_color> render_palette_full(
	paldef const & paldef, coldef const & coldef, byte_t const * in_palette, size_t const in_palette_datasize);

/**
 * @brief Returns a collection of basic tiles (tileset) from the given bitmap image
 *
 * @param chrdef Tile encoding definition
 * @param in_bitmap Input bitmap image
 * @param out_chrset Pointer to output basic tileset
 *
 */
void make_tileset(chrdef const & chrdef, image const & in_bitmap, byte_t * out_chrset);

} // namespace chrgfx

#endif
