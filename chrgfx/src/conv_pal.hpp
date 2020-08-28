#ifndef CHRGFX__CONV_PAL_H
#define CHRGFX__CONV_PAL_H

#include "conv_col.hpp"
#include "gfxdef.hpp"
#include "types.hpp"
#include "utils.hpp"
#include <algorithm>

namespace chrgfx
{
namespace conv_pal
{

/**
 * Returns a paldef palette from standard palette data
 */
u8 *to_paldef_palette(paldef const &to_paldef, coldef const &to_coldef,
											png::palette data, signed int const subpal_idx);

/**
 * Returns a standard palette from paldef palette data
 */
png::palette from_paldef_palette(paldef const &from_paldef,
																 coldef const &from_coldef, u8 const *data,
																 signed int const subpal_idx);

} // namespace conv_pal
} // namespace chrgfx

#endif