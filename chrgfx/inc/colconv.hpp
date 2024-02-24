#ifndef __CHRGFX__COLCONV_HPP
#define __CHRGFX__COLCONV_HPP

#include "coldef.hpp"
#include "stdgfx.hpp"
#include "types.hpp"

namespace chrgfx
{

/**
 * @brief Encode a color with the given coldef
 *
 * @param rgbcoldef RGB color encoding
 * @param color color data
 * @return u32 encoded color
 */
uint32 encode_col(rgbcoldef const & rgbcoldef, color const & color);

/**
 * @brief Encode a color with the given coldef
 *
 * @param refcoldef Non-RGB (reftab) color encoding
 * @param color color data
 * @return u32 encoded color
 */
uint32 encode_col(refcoldef const & refcoldef, color const & color);

/**
 * @brief Decode a color with the given coldef
 *
 * @param rgbcoldef RGB color encoding
 * @param color color data
 * @return basic color
 */
color decode_col(rgbcoldef const & rgbcoldef, uint32 const color);

/**
 * @brief Decode a color with the given coldef
 *
 * @param refcoldef Non-RGB (reftab) color encoding
 * @param color color data
 * @return basic color
 */
color decode_col(refcoldef const & refcoldef, uint32 const color);

} // namespace chrgfx

#endif