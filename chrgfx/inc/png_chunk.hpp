#ifndef CHRGFX__PNG_CHUNK_HPP
#define CHRGFX__PNG_CHUNK_HPP

#include "blob.hpp"
#include "pixbuf.hpp"
#include <cstddef>

namespace chrgfx
{

/**
 * @brief Returns a collection of tiles in the specified chrdef format using the
 * given bitmap
 */
motoi::blob<std::byte> png_chunk(size_t const tile_width, size_t const tile_height, pixbuf const & bitmap);

} // namespace chrgfx

#endif