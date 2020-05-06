#ifndef CHRGFX__CONV_CHR_H
#define CHRGFX__CONV_CHR_H

#include "gfxdef.hpp"
#include "types.hpp"


namespace chrgfx
{

u8 *to_defchr(chr_def const &chrdef, u8 *data);

u8 *to_rawchr(chr_def const &chrdef, u8 *data);

} // namespace chrgfx

#endif