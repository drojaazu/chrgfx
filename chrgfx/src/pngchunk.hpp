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

chrbank pngchunk(chrdef const &chrdef,
								 png::pixel_buffer<png::index_pixel> &bitmap);

}

#endif