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
class coldef;
class paldef;

namespace conv_color
{
/**
 * Represents a function to convert a color to a given encoding
 */
typedef u32 (*colconv_to_t)(coldef const &, png::color const);

/**
 * Represents a function to convert a color from a given encoding
 */
typedef png::color (*colconv_from_t)(coldef const &, u32 const);

/**
 * Reference of all functions for converting a color to a given encoding
 */
extern std::map<std::string const, colconv_to_t> const converters_to;

/**
 * Reference of all functions for converting a color from a given encoding
 */
extern std::map<std::string const, colconv_from_t> const converters_from;

/**
 * Returns a coldef-encoded color from basic color data
 */
u32 colconv_to(coldef const &to_coldef, png::color const data);

/**
 * Returns a basic color from coldef-encoded color data
 */
png::color colconv_from(coldef const &coldef, u32 const data);

} // namespace conv_color
} // namespace chrgfx

#endif