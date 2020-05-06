#ifndef CHRGFX__CONV_PAL_H
#define CHRGFX__CONV_PAL_H

#include "gfxdef.hpp"
#include "types.hpp"
#include "utils.hpp"
#include <algorithm>

namespace chrgfx
{

png::color calc_color(col_def const &coldef, u32 data);

png::palette to_rawpal(pal_def const &paldef, col_def const &coldef, u8 *data,
											 s16 subpal_idx);

} // namespace chrgfx

#endif