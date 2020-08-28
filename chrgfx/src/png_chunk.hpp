#ifndef CHRGFX__PNGCHUNK_H
#define CHRGFX__PNGCHUNK_H

#include "chrbank.hpp"
#include "gfxdef.hpp"
#include "types.hpp"
#include "utils.hpp"
#include <algorithm>
#include <png++/png.hpp>

namespace chrgfx
{

/**
 * Returns a collection of tiles in the specified chrdef format using the given
 * bitmap
 */
chrbank png_chunk(chrdef const &chrdef,
								 png::pixel_buffer<png::index_pixel> const &bitmap);

} // namespace chrgfx

#endif