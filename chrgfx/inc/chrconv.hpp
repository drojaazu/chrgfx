#ifndef __CHRGFX__CHRCONV_HPP
#define __CHRGFX__CHRCONV_HPP

#include "chrdef.hpp"
#include <cstddef>

namespace chrgfx
{

/**
 * @brief Encode a tile with the given chrdef
 *
 * @param chrdef tile encoding
 * @param tile pointer to basic tile data
 * @param out pointer to output memory space; if null, memory will be allocated
 * @return byte_t* pointer to output encoded tile
 */
std::byte * encode_chr(chrdef const & chrdef, std::byte const * tile, std::byte * out = nullptr);

/**
 * @brief Decode a tile with the given chrdef
 *
 * @param chrdef Tile encoding
 * @param tile Pointer to encoded tile data
 * @param out Pointer to output memory space; if null, memory will be allocated
 * @return byte_t* pointer to output basic tile
 */
std::byte * decode_chr(chrdef const & chrdef, std::byte const * tile, std::byte * out = nullptr);

} // namespace chrgfx

#endif