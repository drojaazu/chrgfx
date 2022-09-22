#ifndef CHRGFX__PNG_CHUNK_HPP
#define CHRGFX__PNG_CHUNK_HPP

#include "buffer.hpp"
#include "chrdef.hpp"
#include "types.hpp"
#include <png++/png.hpp>

namespace chrgfx
{

/**
 * @brief Returns a collection of tiles in the specified chrdef format using the
 * given bitmap
 */
buffer<byte_t> png_chunk(size_t const tile_width,
	size_t const tile_height,
	png::pixel_buffer<png::index_pixel> const & bitmap);

} // namespace chrgfx

#endif