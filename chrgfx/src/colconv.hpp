#ifndef CHRGFX__CONV_COL_H
#define CHRGFX__CONV_COL_H

#include "coldef.hpp"
#include "types.hpp"
#include "utils.hpp"
#include <png++/png.hpp>

namespace chrgfx
{

using namespace png;

/**
 * Convert a basic color to a formatted color with the given rgbcoldef
 */
u32 to_formatted_rgbcolor(rgbcoldef const & rgbcoldef, color const & basic_color);

/**
 * Convert a formatted color to a basic color with the given rgbcoldef
 */
color to_basic_rgbcolor(rgbcoldef const & rgbcoldef, u32 const formatted_color);

/**
 * Convert a basic color to a formatted color with the given refcoldef
 */
ushort to_formatted_refcolor(refcoldef const & refcoldef,
													 color const & basic_color);

/**
 * Convert a formatted color to a basic color with the given refcoldef
 */
color to_basic_ref_color(refcoldef const & refcoldef,
											ushort const formatted_color);

} // namespace chrgfx

#endif