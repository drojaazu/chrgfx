#ifndef CHRGFX__CONV_PAL_H
#define CHRGFX__CONV_PAL_H

#include "gfxdef.hpp"
#include "types.hpp"
#include "utils.hpp"
#include <algorithm>

namespace chrgfx
{

png::color from_color(class coldef const &coldef, u32 const data);

u32 to_color(coldef const &to_coldef, png::color const data);

png::palette from_pal(paldef const &from_paldef, coldef const &from_coldef,
											u8 const *data, s16 const subpal_idx);

u8 *to_pal(paldef const &to_paldef, coldef const &to_coldef, png::palette data,
					 s16 const subpal_idx);

} // namespace chrgfx

#endif