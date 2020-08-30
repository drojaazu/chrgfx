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
 * Represents a "from basic" palette converter
 */
typedef u8 *(*palconv_from_t)(paldef const &, coldef const &,
															png::palette const &,
															std::optional<unsigned int const>);

/**
 * Represents a "to basic" palette converter
 */
typedef png::palette (*palconv_to_t)(paldef const &, coldef const &, u8 const *,
																		 std::optional<unsigned int const>);

/**
 * Reference of all functions to convert palettes to basic
 * encoding
 */
extern std::map<string const, palconv_to_t> converters_to;

/**
 * Reference of all functions to convert palettes to secondary encoding
 */
extern std::map<string const, palconv_from_t> converters_from;

/**
 * Returns a paldef-encoded palette from basic palette data
 */
u8 *palconv_from(paldef const &to_paldef, coldef const &to_coldef,
								 png::palette const &data,
								 std::optional<unsigned int const> subpal_idx = std::nullopt);

/**
 * Returns a basic palette from paldef-encoded palette data
 */
png::palette
palconv_to(paldef const &from_paldef, coldef const &from_coldef, u8 const *data,
					 std::optional<unsigned int const> subpal_idx = std::nullopt);

} // namespace conv_palette
} // namespace chrgfx

#endif