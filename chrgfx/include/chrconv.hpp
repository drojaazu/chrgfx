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
byte_t * to_formatted_chr(chrdef const & chrdef, byte_t const * basic_chr);

/**
 * Convert a formatted tile to a basic tile with the given chrdef
 */
byte_t * to_basic_chr(chrdef const & chrdef, byte_t const * formatted_chr);

} // namespace chrgfx

#endif