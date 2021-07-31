#ifndef CHRGFX__PNGCHUNK_H
#define CHRGFX__PNGCHUNK_H

#include "gfxdef.hpp"
#include "types.hpp"
#include "utils.hpp"
#include <algorithm>
#include <png++/png.hpp>
#include <vector>

class buffer;

namespace chrgfx
{

	/**
	 * Returns a collection of tiles in the specified chrdef format using the
	 * given bitmap
	 */
	std::vector<buffer>
	png_chunk(chrdef const & chrdef,
						png::pixel_buffer<png::index_pixel> const & bitmap);

} // namespace chrgfx

#endif