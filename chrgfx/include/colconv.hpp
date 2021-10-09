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
 * \return rgbcoldef formatted color from basic color data
 */
u32 toFormattedRgbColor(rgbcoldef const & color_def, color const & data);

/**
 * Returns a basic color from coldef-encoded color data
 */
color toBasicRgbColor(rgbcoldef const & coldef, u32 const data);

ushort toFormattedRefColor(refcoldef const & color_def, color const & data);

color toBasicRefColor(refcoldef const & color_def, ushort const data);

} // namespace chrgfx

#endif