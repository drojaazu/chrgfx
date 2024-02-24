#ifndef CHRGFX__IMGFMT_PNG_HPP
#define CHRGFX__IMGFMT_PNG_HPP

#include "blob.hpp"
#include "pixbuf.hpp"
#include "types.hpp"
#include <png++/pixel_buffer.hpp>
#include <png++/png.hpp>

namespace chrgfx
{

/**
 * @brief Returns a collection of tiles in the specified chrdef format using the
 * given bitmap
 */
motoi::blob<byte_t> make_tileset(size_t const tile_width, size_t const tile_height, image const & bitmap);

chrgfx::image make_pixbuf_from_png(png::pixel_buffer<png::index_pixel> const & png_pixbuf);

chrgfx::palette make_palette_from_png(png::palette const & png_pixbuf);
} // namespace chrgfx

#endif
