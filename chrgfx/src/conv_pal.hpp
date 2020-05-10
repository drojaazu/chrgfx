#ifndef CHRGFX__CONV_PAL_H
#define CHRGFX__CONV_PAL_H

#include "gfxdef.hpp"
#include "types.hpp"
#include "utils.hpp"
#include <algorithm>

namespace chrgfx
{

png::color calc_color(class coldef const &coldef, u32 const data);

png::palette from_pal(paldef const &from_paldef, coldef const &from_coldef,
											u8 *data, s16 const subpal_idx);

} // namespace chrgfx

#endif