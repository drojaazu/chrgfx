#ifndef __CHRGFX__COLCONV_HPP
#define __CHRGFX__COLCONV_HPP

#include "coldef.hpp"
#include "types.hpp"
#include <png++/png.hpp>

namespace chrgfx
{

/**
 * Convert a basic color to a formatted color with the given rgbcoldef
 */
u32 encode_col(rgbcoldef const & rgbcoldef, png::color const & color);

u32 encode_col(refcoldef const & refcoldef, png::color const & color);

/**
 * Convert a formatted color to a basic color with the given rgbcoldef
 */
png::color decode_col(rgbcoldef const & rgbcoldef, u32 const color);

png::color decode_col(refcoldef const & refcoldef, u32 const color);

} // namespace chrgfx

#endif