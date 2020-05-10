#ifndef CHRGFX__CONV_CHR_H
#define CHRGFX__CONV_CHR_H

#include "gfxdef.hpp"
#include "types.hpp"
#include <iomanip>

namespace chrgfx
{

u8 *to_chr(chrdef const &to_def, u8 const *data);

u8 *from_chr(chrdef const &from_def, u8 const *data);

} // namespace chrgfx

#endif