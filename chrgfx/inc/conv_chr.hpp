#ifndef CHRGFX__CONV_CHR_H
#define CHRGFX__CONV_CHR_H

#include "gfxdef.hpp"
#include "types.hpp"
#include <iomanip>

namespace chrgfx
{
namespace conv_chr
{

/*
TILE CONVERSION ROUTINES
Any custom tile conversion functions should take a reference to a chr_def and
a pointer to the array of raw byte data. It will return an array of converted
byte data.

The converted byte data should be an array of unsigned values, each as one
pixel referencing a value in the output palette.
*/

/**
 * Returns a chrdef tile from standard tile data
 */
u8 *to_chrdef_chr(chrdef const &to_def, u8 const *data);

/**
 * Returns a standard tile from chrdef tile data
 */
u8 *from_chrdef_chr(chrdef const &from_def, u8 const *data);

} // namespace conv_chr
} // namespace chrgfx

#endif