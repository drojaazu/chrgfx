#ifndef CHRGFX__PNGPAL_H
#define CHRGFX__PNGPAL_H

#include "conv_pal.hpp"
#include "gfxdef.hpp"
#include "types.hpp"
#include <png++/png.hpp>

namespace chrgfx
{
u8 *pngpal(paldef const &paldef, coldef const &coldef, png::palette &palette);
}

#endif