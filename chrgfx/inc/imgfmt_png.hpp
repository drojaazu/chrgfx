/**
 * @file imgfmt_png.hpp
 * @author Damian Rogers / damian@motoi.pro
 * @copyright ©2022 Motoi Productions / Released under MIT License
 * @brief Functions for converting to/from PNG format
 */

#ifndef __CHRGFX__IMGFMT_PNG_HPP
#define __CHRGFX__IMGFMT_PNG_HPP

#include "basic_gfx.hpp"
#include "types.hpp"
#include <optional>
#include <png++/pixel_buffer.hpp>
#include <png++/png.hpp>

namespace chrgfx
{
chrgfx::basic_image from_png(png::image<png::index_pixel> const & png_image);

png::image<png::index_pixel> to_png(
	chrgfx::basic_image const & basic_image, std::optional<uint8> trns_index = std::nullopt);

} // namespace chrgfx

#endif
