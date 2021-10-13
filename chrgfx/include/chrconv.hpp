#ifndef CHRGFX__CONV_CHR_H
#define CHRGFX__CONV_CHR_H

#include "buffer.hpp"
#include "chrdef.hpp"
#include "types.hpp"
//#include <iomanip>
#include <map>
#include <string>

namespace chrgfx
{

/**
 * Convert a basic tile to a formatted tile with the given chrdef
 */
byte * to_formatted_chr(chrdef const & chrdef, byte const * basic_chr);

/**
 * Convert a formatted tile to a basic tile with the given chrdef
 */
byte * to_basic_chr(chrdef const & chrdef, byte const * formatted_chr);

} // namespace chrgfx

#endif