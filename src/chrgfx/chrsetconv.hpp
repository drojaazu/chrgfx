/**
 * @file chrsetconv.hpp
 * @author Damian Rogers / damian@motoi.pro
 * @copyright ©2022 Motoi Productions / Released under MIT License
 * @brief Tileset conversion functions
 */

#include "basic_gfx.hpp"
#include "chrdef.hpp"
#include <optional>

#ifndef __CHRGFX__CHRSETCONV_HPP
#define __CHRGFX__CHRSETCONV_HPP

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
basic_image render_chrset(
	chrdef const & chrdef, byte_t const * in_chrset, size_t const in_chrset_datasize, render_config const & render_cfg);

/**
 * @brief Returns a collection of basic tiles (tileset) from the given bitmap image
 *
 * @param chrdef Tile encoding definition
 * @param in_bitmap Input bitmap image
 * @param out_chrset Pointer to output basic tileset
 *
 */
void make_chrset(chrdef const & chrdef, basic_image const & in_bitmap, byte_t * out_chrset);

} // namespace chrgfx

#endif
