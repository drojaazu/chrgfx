/**
 * @file custom.hpp
 * @author Damian Rogers / damian@motoi.pro
 * @copyright ©2022 Motoi Productions / Released under MIT License
 * @brief Custom tile conversion functions
 */

#ifndef __CHRGFX__CUSTOM_HPP
#define __CHRGFX__CUSTOM_HPP

#include "image_types.hpp"
#include "types.hpp"
#include <map>

namespace chrgfx::custom
{

/**
 * This namespace should be used for any non-standard tile, palette or color
 * converters.
 *
 * By non-standard, we mean converters that can not use chrdefs/paldefs/coldefs.
 *
 * For example, tiles that do not have a regular bit pattern or pseudo-retro
 * file formats such as palettes for PC graphics editors.
 *
 * Currently, this is not integrated with the main gfxdef system, and is not accessible
 * with the frontend utilities. This should (hopefully) change in a future version.
 */

using fp_encode_chr = size_t (*)(pixel const * in_chr, byte_t * out_chr);
using fp_decode_chr = void (*)(byte_t const * in_chr, size_t datasize, pixel * out_chr);

using fp_encode_pal = size_t (*)(palette const * in_pal, byte_t * out_pal);
using fp_decode_pal = void (*)(byte_t const * in_pal, size_t datasize, palette * out_pal);

using fp_encode_col = size_t (*)(rgb_color const * in_col, byte_t * out_col);
using fp_decode_col = void (*)(byte_t const * in_col, size_t datasize, rgb_color * out_col);

/**
 * @brief Converts the Nintendo Super Famicom 3bpp "quasi format"
 *
 * @param encoded_chr 8x8 encoded tile (24 bytes)
 * @return byte_t* 8x8 basic tile (64 bytes)
 */
void decode_chr_nintendo_sfc_3bpp(byte_t const * encoded_chr, size_t datasize, pixel * out);

/**
 * @brief Converts an RGB TileLayer Pro palette to a basic palette
 */
void decode_pal_tilelayerpro(byte_t const * in_pal, size_t datasize, palette * out_pal);

/**
 * @brief Converts a basic palette to a TileLayer Pro RGB palette
 */
size_t encode_pal_tilelayerpro(palette const * palette, byte_t * out_pal);

/**
 * @brief Converts a JASC Paint Shop Pro palette to a basic palette
 */
// void decode_pal_paintshoppro(byte_t const * in_pal, size_t datasize, basic_palette * out_pal);

/**
 * @brief Converts a basic palette to a JASC Paint Shop Pro palette
 */
// size_t encode_pal_paintshoppro(basic_palette const * palette, byte_t * out_pal);

std::map<std::string, fp_encode_chr> const custom_chr_encoders;
std::map<std::string, fp_decode_chr> const custom_chr_decoders {{"sfc_3bpp", decode_chr_nintendo_sfc_3bpp}};

std::map<std::string, fp_encode_pal> const custom_pal_encoders {{"tilelayerpro", encode_pal_tilelayerpro}};
std::map<std::string, fp_decode_pal> const custom_pal_decoders {{"tilelayerpro", decode_pal_tilelayerpro}};

std::map<std::string, fp_encode_col> const custom_col_encoders;
std::map<std::string, fp_decode_col> const custom_col_decoders;

} // namespace chrgfx::custom

#endif
