/**
 * @file palconv.hpp
 * @author Damian Rogers / damian@motoi.pro
 * @copyright ©2022 Motoi Productions / Released under MIT License
 * @brief Palette format conversion functions
 */

#ifndef __CHRGFX__PALCONV_HPP
#define __CHRGFX__PALCONV_HPP

#include "basic_gfx.hpp"
#include "coldef.hpp"
#include "paldef.hpp"
#include "types.hpp"

namespace chrgfx
{

/**
 * @brief Encode a basic palette with the given palette definition
 *
 * @param paldef Pointer to palette encoding definition
 * @param coldef Pointer to color encoding definition
 * @param in_palette Pointer to input basic palette
 * @param out_palette Pointer to output encoded palette
 */
void encode_pal(paldef const * paldef, coldef const * coldef, basic_palette const * in_palette, byte_t * out_palette);

/**
 * @brief Decode an encoded palette with the given paldef
 *
 * @param paldef Pointer to palette encoding definition
 * @param coldef Pointer to color encoding definition
 * @param in_palette Pointer to input encoded palette
 * @param out_palette Pointer to output basic palette
 */
void decode_pal(paldef const * paldef, coldef const * coldef, byte_t const * in_palette, basic_palette * out_palette);

} // namespace chrgfx

#endif