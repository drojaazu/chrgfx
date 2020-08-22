#ifndef CHRGFX__CONV_PAL_H
#define CHRGFX__CONV_PAL_H

#include "gfxdef.hpp"
#include "types.hpp"
#include "utils.hpp"
#include <algorithm>

namespace chrgfx
{

/**
 * Returns a coldef color from standard color data
 */
u32 to_color(coldef const &to_coldef, png::color const data);

/**
 * Returns a standard color from coldef color data
 */
png::color from_color(class coldef const &coldef, u32 const data);

/**
 * Returns a paldef palette from standard palette data
 */
u8 *to_pal(paldef const &to_paldef, coldef const &to_coldef, png::palette data,
					 s16 const subpal_idx);

/**
 * Returns a standard palette from paldef palette data
 */
png::palette from_pal(paldef const &from_paldef, coldef const &from_coldef,
											u8 const *data, s16 const subpal_idx);

} // namespace chrgfx

#endif