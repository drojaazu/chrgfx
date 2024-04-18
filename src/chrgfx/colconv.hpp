/**
 * @file colconv.hpp
 * @author Damian Rogers / damian@motoi.pro
 * @copyright ©2022 Motoi Productions / Released under MIT License
 * @brief Color format conversion functions
 */

#ifndef __CHRGFX__COLCONV_HPP
#define __CHRGFX__COLCONV_HPP

#include "basic_gfx.hpp"
#include "coldef.hpp"
#include "types.hpp"

namespace chrgfx
{

/**
 * @brief Encode a color with the given coldef
 *
 * @param rgbcoldef RGB Color encoding
 * @param color Color data
 * @return u32 Encoded color
 */
uint32 encode_col(rgbcoldef const & rgbcoldef, basic_color const & color);

/**
 * @brief Encode a color with the given coldef
 *
 * @param refcoldef Non-RGB (reftab) color encoding
 * @param color Color data
 * @return u32 Encoded color
 */
uint32 encode_col(refcoldef const & refcoldef, basic_color const & color);

/**
 * @brief Decode a color with the given coldef
 *
 * @param rgbcoldef RGB color encoding
 * @param color Color data
 * @return Basic color
 */
basic_color decode_col(rgbcoldef const & rgbcoldef, uint32 const color);

/**
 * @brief Decode a color with the given coldef
 *
 * @param refcoldef Non-RGB (reftab) color encoding
 * @param color Color data
 * @return Basic color
 */
basic_color decode_col(refcoldef const & refcoldef, uint32 const color);

} // namespace chrgfx

#endif