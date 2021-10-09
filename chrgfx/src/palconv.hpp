#ifndef CHRGFX__CONV_PAL_H
#define CHRGFX__CONV_PAL_H

#include "buffer.hpp"
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
u8 * toFormattedPal(paldef const & paldef, rgbcoldef const & coldef,
											 palette const & paldata,
											 optional<unsigned int const> const & subpal_idx);

/**
 * Convert a palette from the specified encoding
 */
palette toBasicPal(paldef const & from_paldef, rgbcoldef const & from_coldef,
											u8 const * data,
											optional<unsigned int const> const & subpal_idx);

} // namespace chrgfx

#endif