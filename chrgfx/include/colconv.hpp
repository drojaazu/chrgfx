#ifndef CHRGFX__CONV_COL_H
#define CHRGFX__CONV_COL_H

#include "types.hpp"
#include "utils.hpp"
#include <map>
#include <string>

namespace chrgfx
{
// forward declaration of gfxdef types
class chrdef;
class rgbcoldef;
class paldef;

namespace converters
{

/**
 * Returns a coldef-encoded color from basic color data
 */
u32 convertToColor(rgbcoldef const &to_coldef, png::color const data);

/**
 * Returns a basic color from coldef-encoded color data
 */
png::color convertFromColor(rgbcoldef const &coldef, u32 const data);

} // namespace conv_color
} // namespace chrgfx

#endif