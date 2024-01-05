#ifndef __CHRGFX__COLCONV_HPP
#define __CHRGFX__COLCONV_HPP

#include "coldef.hpp"
#include "types.hpp"
#include <png++/png.hpp>

namespace chrgfx
{

/**
 * @brief Encode a color with the given coldef
 *
 * @param rgbcoldef RGB color encoding
 * @param color color data
 * @return u32 encoded color
 */
u32 encode_col(rgbcoldef const & rgbcoldef, png::color const & color);

/**
 * @brief Encode a color with the given coldef
 *
 * @param refcoldef Non-RGB (reftab) color encoding
 * @param color color data
 * @return u32 encoded color
 */
u32 encode_col(refcoldef const & refcoldef, png::color const & color);

/**
 * @brief Decode a color with the given coldef
 *
 * @param rgbcoldef RGB color encoding
 * @param color color data
 * @return png::color basic color
 */
png::color decode_col(rgbcoldef const & rgbcoldef, u32 const color);

/**
 * @brief Decode a color with the given coldef
 *
 * @param refcoldef Non-RGB (reftab) color encoding
 * @param color color data
 * @return png::color basic color
 */
png::color decode_col(refcoldef const & refcoldef, u32 const color);

} // namespace chrgfx

#endif