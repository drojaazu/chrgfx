#ifndef CHRGFX__PNGCHUNK_H
#define CHRGFX__PNGCHUNK_H

#include "bank.hpp"
#include "gfxdef.hpp"
#include "types.hpp"
#include "utils.hpp"
#include <algorithm>
#include <png++/png.hpp>

namespace chrgfx
{
bank pngchunk(png::image<png::index_pixel> &bitmap, chr_def const &chrdef);

}

#endif