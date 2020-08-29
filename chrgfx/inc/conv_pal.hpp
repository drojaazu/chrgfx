#ifndef CHRGFX__CONV_PAL_H
#define CHRGFX__CONV_PAL_H

#include "gfxdef.hpp"
#include "types.hpp"
#include "utils.hpp"
#include <algorithm>
#include <map>

namespace chrgfx
{
namespace conv_palette
{
/**
 * Returns a paldef-encoded palette from basic palette data
 */
u8 *palconv_from(paldef const &to_paldef, coldef const &to_coldef,
								 png::palette const &data, signed int const subpal_idx);

/**
 * Returns a basic palette from paldef-encoded palette data
 */
extern png::palette palconv_to(paldef const &from_paldef,
															 coldef const &from_coldef, u8 const *data,
															 signed int const subpal_idx);

/**
 * Reference of all functions to convert palettes to basic
 * encoding
 */
extern std::map<string const, png::palette (*)(paldef const &, coldef const &,
																							 u8 const *, signed int const)>
		converters_to;

/**
 * Reference of all functions to convert palettes to secondary encoding
 */
extern std::map<string const, u8 *(*)(paldef const &, coldef const &,
																			png::palette const &, signed int const)>
		converters_from;
} // namespace conv_palette
} // namespace chrgfx

#endif