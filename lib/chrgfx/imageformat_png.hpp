/**
 * @file imageformat_png.hpp
 * @author Damian Rogers / damian@motoi.pro
 * @copyright ©2022 Motoi Productions / Released under MIT License
 * @brief Functions for converting to/from PNG format
 */

#ifndef __CHRGFX__IMAGEFORMAT_PNG_HPP
#define __CHRGFX__IMAGEFORMAT_PNG_HPP

#include "image_types.hpp"
#include "types.hpp"
#include <optional>
#include <png++/pixel_buffer.hpp>
#include <png++/png.hpp>

namespace chrgfx
{
image from_png(png::image<png::index_pixel> const & png_image);

png::image<png::index_pixel> to_png(image const & basic_image, std::optional<uint8> trns_index = std::nullopt);

} // namespace chrgfx

#endif
