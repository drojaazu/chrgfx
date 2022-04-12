#ifndef CHRGFX__PALCONV_HPP
#define CHRGFX__PALCONV_HPP

#include "coldef.hpp"
#include "paldef.hpp"
#include "types.hpp"
#include <png++/png.hpp>
#include <string>

namespace chrgfx
{

/**
 * Encode a palette with the given paldef
 */
byte_t * encode_pal(paldef const & paldef, coldef const & coldef,
										png::palette const & palette);

/**
 * Decode an encoded palette with the given paldef
 */
png::palette decode_pal(paldef const & paldef, coldef const & coldef,
												byte_t const * palette);

} // namespace chrgfx

#endif