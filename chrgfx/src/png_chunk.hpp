#ifndef CHRGFX__PNGCHUNK_H
#define CHRGFX__PNGCHUNK_H

#include "buffer.hpp"
#include "chrdef.hpp"
#include "gfxdef.hpp"
#include "types.hpp"
#include "utils.hpp"
#include <algorithm>
#include <png++/png.hpp>
#include <vector>

using png::index_pixel;
using png::pixel_buffer;
using std::vector;

namespace chrgfx
{

/**
 * Returns a collection of tiles in the specified chrdef format using the
 * given bitmap
 */
vector<uptr<byte>> png_chunk(chrdef const & chrdef,
														 pixel_buffer<index_pixel> const & bitmap);

} // namespace chrgfx

#endif