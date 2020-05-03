#ifndef PAL_CONV_H
#define PAL_CONV_H
#include "types.hpp"

namespace chrgfx
{
png::palette to_rawpal(pal_def &paldef, chunk data, s16 subpal_idx = -1);

chunk to_defpal(pal_def &paldef, png::palette data, s16 subpal_idx = -1);

png::color to_rawcol(col_def &coldef, u32 data);

u32 to_defcol(col_def &coldef, png::color data);

png::color calc_color(col_def &coldef, u32 data);

} // namespace chrgfx
#endif