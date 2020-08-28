#ifndef CHRGFX__CONV_COL_H
#define CHRGFX__CONV_COL_H

#include "gfxdef.hpp"
#include "types.hpp"
#include "utils.hpp"

namespace chrgfx
{
namespace conv_col
{
/**
 * Returns a coldef color from standard color data
 */
u32 to_coldef_color(coldef const &to_coldef, png::color const data);

/**
 * Returns a standard color from coldef color data
 */
png::color from_coldef_color(coldef const &coldef, u32 const data);

} // namespace conv_col
} // namespace chrgfx

#endif