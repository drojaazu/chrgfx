#ifndef __CHRGFX__CUSTOM_HPP
#define __CHRGFX__CUSTOM_HPP

#include "types.hpp"
#include <png++/png.hpp>

namespace chrgfx
{

/**
 * This namespace should be used for any non-standard tile, palette or color
 * converters.
 *
 * By non-standard, we mean converters that can not use chrdefs/paldefs/coldefs.
 *
 * For example, tiles that do not have a regular bit pattern or psuedo-retro
 * file formats such as palettes for graphics editors.
 */
namespace custom
{

/**
 * @brief Converts the Nintendo Super Famicom 3bpp "quasi format"
 *
 * @param encoded_chr 8x8 encoded tile (24 bytes)
 * @return byte_t* 8x8 basic tile (64 bytes)
 */
byte_t * decode_chr_nintendo_sfc_3bpp(byte_t const * encoded_chr,
																			byte_t * out = nullptr);

/**
 * @brief Converts an RGB TileLayer Pro palette to a basic palette
 */
png::palette decode_pal_tilelayerpro(std::istream & tpl_palette);

/**
 * @brief Converts a basic palette to a TileLayer Pro RGB palette
 */
void encode_pal_tilelayerpro(png::palette const & palette,
														 std::ostream & output);

/**
 * @brief Converts a JASC Paint Shop Pro palette to a basic palette
 */
png::palette decode_pal_paintshoppro(std::istream & psp_palette);

/**
 * @brief Converts a basic palette to a JASC Paint Shop Pro palette
 */
void encode_pal_paintshoppro(png::palette const & palette,
														 std::ostream & output);

} // namespace custom
} // namespace chrgfx

#endif
