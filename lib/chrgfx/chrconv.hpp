/**
 * @file chrconv.hpp
 * @author Damian Rogers / damian@motoi.pro
 * @copyright ©2022 Motoi Productions / Released under MIT License
 * @brief Tile format conversion functions
 */

#ifndef __CHRGFX__CHRCONV_HPP
#define __CHRGFX__CHRCONV_HPP

#include "chrdef.hpp"
#include "image_types.hpp"
#include "types.hpp"

namespace chrgfx
{

/**
 * @brief Encode a basic tile with the given tile definition
 *
 * @param chrdef Pointer to tile encoding definition
 * @param in_tile Pointer to input basic tile
 * @param out_tile Pointer to output encoded tile
 *
 */
void encode_chr(chrdef const & chrdef, pixel const * in_tile, byte_t * out_tile);

/**
 * @brief Decode an encoded tile with the given tile definition
 *
 * @param chrdef Pointer to tile encoding definition
 * @param in_tile Pointer to input encoded tile
 * @param out_tile Pointer to output basic tile
 *
 */
void decode_chr(chrdef const & chrdef, byte_t const * in_tile, pixel * out_tile);

} // namespace chrgfx

#endif