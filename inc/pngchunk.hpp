#ifndef PNGCHUNK_H
#define PNGCHUNK_H

#include "utils.hpp"

using namespace png;

namespace chrgfx
{
bank *pngchunk(image<index_pixel> &bitmap, chr_def &chrdef);
}

#endif