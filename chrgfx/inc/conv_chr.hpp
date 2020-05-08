#ifndef CHR_CONV_H
#define CHR_CONV_H

#include "gfxdef.hpp"
#include "types.hpp"
namespace chrgfx
{

/**
 * Returns a CHR from raw packed pixel data
 */
u8 *to_defchr(chr_def const &chrdef, u8 *data);

/**
 * Returns raw packed pixel data from a CHR
 */
u8 *to_rawchr(chr_def const &chrdef, u8 *data);

} // namespace chrgfx

#endif
