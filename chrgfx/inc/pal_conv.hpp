#ifndef PAL_CONV_H
#define PAL_CONV_H
#include "types.hpp"
#include <png++/png.hpp>

namespace chrgfx
{
png::palette to_rawpal(pal_def const &paldef, col_def const &coldef, u8 *data,
											 s16 subpal_idx);

chunk to_defpal(pal_def const &paldef, png::palette data, s16 subpal_idx = -1);

png::color to_rawcol(col_def const &coldef, u32 data);

u32 to_defcol(col_def const &coldef, png::color data);

} // namespace chrgfx
#endif