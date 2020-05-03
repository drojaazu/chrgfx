#ifndef PNGCHUNK_H
#define PNGCHUNK_H
#include "types.hpp"

namespace chrgfx
{

bank *pngchunk(png::image<png::index_pixel> &bitmap, chr_def &chrdef);

} // namespace chrgfx

#endif