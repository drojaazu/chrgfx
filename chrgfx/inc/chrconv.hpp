#ifndef __CHRGFX__CHRCONV_HPP
#define __CHRGFX__CHRCONV_HPP

#include "chrdef.hpp"
#include "types.hpp"

namespace chrgfx
{

/**
 * @brief Encode a tile with the given chrdef
 *
 * @param chrdef Tile encoding
 * @param tile Pointer to basic tile data
 * @param out Pointer to output memory space; if null, memory will be allocated
 * @return byte_t* Pointer to the output tile
 */
byte_t * encode_chr(chrdef const & chrdef, byte_t const * tile,
										byte_t * out = nullptr);

/**
 * @brief Decode a tile with the given chrdef
 *
 * @param chrdef Tile encoding
 * @param tile Pointer to encoded tile data
 * @param out Pointer to output memory space; if null, memory will be allocated
 * @return byte_t* Pointer to the output tile
 */
byte_t * decode_chr(chrdef const & chrdef, byte_t const * tile,
										byte_t * out = nullptr);

} // namespace chrgfx

#endif