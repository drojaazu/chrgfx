#ifndef CHRGFX__IMGFMT_PNG_HPP
#define CHRGFX__IMGFMT_PNG_HPP

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
