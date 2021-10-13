#ifndef CHRGFX__CONV_PAL_H
#define CHRGFX__CONV_PAL_H

#include "colconv.hpp"
#include "coldef.hpp"
#include "paldef.hpp"
#include "types.hpp"
#include "utils.hpp"
#include <algorithm>
#include <map>
#include <optional>
#include <string>

using namespace std;

namespace chrgfx
{

/**
 * Convert a palette to the specified encoding
 */
buffer to_formatted_palette(paldef const & paldef, rgbcoldef const & rgbcoldef,
											palette const & basic_palette,
											const ushort subpal_idx = 0);

/**
 * Convert a palette from the specified encoding
 */
palette to_basic_palette(paldef const & paldef, rgbcoldef const & rgbcoldef,
									 buffer const & formatted_palette,
									 const ushort subpal_idx = 0);

/**
 * Convert a palette to the specified encoding
 */
u8 * toFormattedPal(paldef const & paldef, refcoldef const & refcoldef,
										palette const & paldata,
										optional<unsigned int const> const & subpal_idx);

/**
 * Convert a palette from the specified encoding
 */
palette toBasicPal(paldef const & from_paldef, refcoldef const & refcoldef,
									 u8 const * data,
									 optional<unsigned int const> const & subpal_idx);

} // namespace chrgfx

#endif