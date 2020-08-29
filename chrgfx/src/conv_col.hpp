#ifndef CHRGFX__CONV_COL_H
#define CHRGFX__CONV_COL_H

#include "gfxdef.hpp"
#include "types.hpp"
#include "utils.hpp"
#include <map>

namespace chrgfx
{
namespace conv_color
{

/**
 * Returns a coldef-encoded color from basic color data
 */
u32 colconv_from(coldef const &to_coldef, png::color const data);

/**
 * Returns a basic color from coldef-encoded color data
 */
png::color colconv_to(coldef const &coldef, u32 const data);

/**
 * Reference of all functions to convert colors to basic encoding
 */
extern std::map<string const, png::color (*)(coldef const &, u32 const)>
		converters_to;

/**
 * Reference of all functions to convert colors to secondary encoding
 */
extern std::map<string const, u32 (*)(coldef const &, png::color const)>
		converters_from;

} // namespace conv_color
} // namespace chrgfx

#endif