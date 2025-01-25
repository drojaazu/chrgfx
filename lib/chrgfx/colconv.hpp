/**
 * @file colconv.hpp
 * @author Damian Rogers / damian@motoi.pro
 * @copyright ©2022 Motoi Productions / Released under MIT License
 * @brief Color format conversion functions
 */

#ifndef __CHRGFX__COLCONV_HPP
#define __CHRGFX__COLCONV_HPP

#include "coldef.hpp"
#include "image.hpp"
#include "image_types.hpp"
#include "types.hpp"

namespace chrgfx
{

/**
 * @brief Encode a basic color with the given RGB color definition
 *
 * @param rgbcoldef Pointer to RGB color definition
 * @param in_color Pointer to input basic color
 * @param out_color Pointer to output encoded color
 */
void encode_col(rgbcoldef const * rgbcoldef, rgb_color const * in_color, uint32 * out_color);

/**
 * @brief Encode a basic color with the given reference palette
 *
 * @param refcoldef Pointer to reference palette
 * @param in_color Pointer to input basic color
 * @param out_color Pointer to output encoded color
 */
void encode_col(refcoldef const * refcoldef, rgb_color const * in_color, uint32 * out_color);

/**
 * @brief Decode an encoded color with the given RGB color definition
 *
 * @param rgbcoldef Pointer to RGB color definition
 * @param in_color Pointer to input encoded color
 * @param out_color Pointer to output basic color
 */
void decode_col(rgbcoldef const * rgbcoldef, uint32 const * in_color, rgb_color * out_color);

/**
 * @brief Decode an encoded color with the given reference palette
 *
 * @param refcoldef Pointer to reference palette
 * @param in_color Pointer to input encoded color
 * @param out_color Pointer to output basic color
 */
void decode_col(refcoldef const * refcoldef, uint32 const * in_color, rgb_color * out_color);

} // namespace chrgfx

#endif