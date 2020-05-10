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

chrbank pngchunk(png::image<png::index_pixel> &bitmap, chrdef const &chrdef);

}

#endif