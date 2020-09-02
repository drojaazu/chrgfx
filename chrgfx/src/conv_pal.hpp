#ifndef CHRGFX__CONV_PAL_H
#define CHRGFX__CONV_PAL_H

#include "conv_col.hpp"
#include "types.hpp"
#include "utils.hpp"
#include <algorithm>
#include <map>
#include <optional>
#include <string>

namespace chrgfx
{
// forward declaration of gfxdef types
class chrdef;
class coldef;
class paldef;

namespace conv_palette
{
/**
 * Represents a function to convert a palette to a given encoding
 */
typedef u8 *(*cvto_pal_t)(paldef const &, coldef const &, png::palette const &,
													std::optional<unsigned int const> const &,
													std::optional<conv_color::cvto_col_t> const &);

/**
 * Represents a function to convert a palette from a given encoding
 */
typedef png::palette (*cvfrom_pal_t)(
		paldef const &, coldef const &, u8 const *,
		std::optional<unsigned int const> const &,
		std::optional<conv_color::cvfrom_col_t> const &);

/**
 * Reference of all functions for converting a palette to a given encoding
 */
extern std::map<std::string const, cvto_pal_t> const converters_to;

/**
 * Reference of all functions for converting a palette from a given encoding
 */
extern std::map<std::string const, cvfrom_pal_t> const converters_from;

/**
 * Convert a palette to the specified encoding
 */
u8 *cvto_pal(
		paldef const &to_paldef, coldef const &to_coldef, png::palette const &data,
		std::optional<unsigned int const> const &subpal_idx = std::nullopt,
		std::optional<conv_color::cvto_col_t> const &color_conv = std::nullopt);

/**
 * Convert a palette from the specified encoding
 */
png::palette cvfrom_pal(
		paldef const &from_paldef, coldef const &from_coldef, u8 const *data,
		std::optional<unsigned int const> const &subpal_idx = std::nullopt,
		std::optional<conv_color::cvfrom_col_t> const &color_conv = std::nullopt);

/**
 * Convert a palette from TileLayer Pro format
 */
png::palette cvfrom_tilelayerpro_pal(
		paldef const &from_paldef, coldef const &from_coldef, u8 const *data,
		std::optional<unsigned int const> const &subpal_idx,
		std::optional<conv_color::cvfrom_col_t> const &color_conv);

} // namespace conv_palette
} // namespace chrgfx

#endif