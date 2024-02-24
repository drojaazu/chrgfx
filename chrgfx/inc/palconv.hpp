#ifndef CHRGFX__PALCONV_HPP
#define CHRGFX__PALCONV_HPP

#include "coldef.hpp"
#include "paldef.hpp"
#include "pixbuf.hpp"
#include "types.hpp"

namespace chrgfx
{

/**
 * @brief Encode a palette with the given paldef
 *
 * @param paldef palette encoding
 * @param coldef color encoding
 * @param palette palette data
 * @return byte_t* pointer to output encoded palette
 */
byte_t * encode_pal(paldef const & paldef, coldef const & coldef, palette const & palette);

/**
 * @brief Decode an encoded palette with the given paldef
 *
 * @param paldef palette encoding
 * @param coldef color encoding
 * @param palette palette data
 * @return png::palette
 */
palette decode_pal(paldef const & paldef, coldef const & coldef, byte_t const * palette);

} // namespace chrgfx

#endif