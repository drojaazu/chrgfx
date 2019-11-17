#ifndef PAL_CONV_H
#define PAL_CONV_H
#include "types.hpp"

namespace chrgfx
{

palette *get_pal(pal_def &paldef, bptr data, color (*get_color)(pal_def &, u32),
								 s16 subpal_idx);

/**
 * Returns a palette using color definitions
 */
palette *get_pal_coldef(pal_def &paldef, bptr data, s16 subpal_idx);

/**
 * Returns a palette using a reference palette
 */
palette *get_pal_refpal(pal_def &paldef, bptr data, s16 subpal_idx);

/**
 * Returns a palette based on TileLayer Pro palette data
 */
palette *get_pal_tlp(pal_def &paldef, bptr data, s16 subpal_idx);

color calc_color(col_def &coldef, u32 data);

static const std::map<const std::string, pal_cv> pal_converters{
		{std::string("coldef"), get_pal_coldef},
		{std::string("refpal"), get_pal_refpal},
		{std::string("tlp"), get_pal_tlp}};

} // namespace chrgfx
#endif