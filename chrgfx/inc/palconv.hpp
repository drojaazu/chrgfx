/**
 * @file palconv.hpp
 * @author Damian Rogers / damian@motoi.pro
 * @copyright ©2022 Motoi Productions / Released under MIT License
 * @brief Palette format conversion functions
 */

#ifndef __CHRGFX__PALCONV_HPP
#define __CHRGFX__PALCONV_HPP

#include "basic_gfx.hpp"
#include "coldef.hpp"
#include "paldef.hpp"
#include "types.hpp"

namespace chrgfx
{

/**
 * @brief Encode a palette with the given paldef
 *
 * @param paldef palette encoding
 * @param coldef color encoding
 * @param palette palette data
 * @return byte_t* pointer to output encoded palette
 */
byte_t * encode_pal(paldef const & paldef, coldef const & coldef, basic_palette const & palette);

/**
 * @brief Decode an encoded palette with the given paldef
 *
 * @param paldef palette encoding
 * @param coldef color encoding
 * @param palette palette data
 * @return png::palette
 */
basic_palette decode_pal(paldef const & paldef, coldef const & coldef, byte_t const * palette);

} // namespace chrgfx

#endif