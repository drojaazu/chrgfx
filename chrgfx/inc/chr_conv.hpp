#ifndef CHR_CONV_H
#define CHR_CONV_H

#include "types.hpp"
#include "gfxdef.hpp"
namespace chrgfx
{

/**
 * Returns a CHR from raw packed pixel data
 */
defchr to_defchr(chr_def &chrdef, rawchr data);

/**
 * Returns raw packed pixel data from a CHR
 */
rawchr to_rawchr(chr_def &chrdef, defchr data);

} // namespace chrgfx

#endif
