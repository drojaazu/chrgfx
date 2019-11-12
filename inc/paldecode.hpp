#ifndef PALDECODE_H
#define PALDECODE_H
#include "global.hpp"

namespace chrgfx
{
palette *pal_decode(pal_def &paldef, bptr data,
										color (*get_color)(pal_def &, u32), s16 subpal_idx);

// hardware that uses RGB to create colors
// uses colordef to return colors
palette *pal_decode_calc(pal_def &paldef, bptr data, s16 subpal_idx);

// hardware with a fixed system palette
// uses syspal to return colors
palette *pal_decode_fixed(pal_def &paldef, bptr data, s16 subpal_idx);

// Tile Layer Pro
palette *pal_decode_soft_tlp(pal_def &paldef, bptr data, s16 subpal_idx);

color calc_color(color_def &colordef, u32 data);

} // namespace chrgfx
#endif