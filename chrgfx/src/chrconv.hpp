#ifndef CHRGFX__CONV_CHR_H
#define CHRGFX__CONV_CHR_H

#include "buffer.hpp"
#include "chrdef.hpp"
#include "types.hpp"
#include <map>
#include <string>

namespace chrgfx
{

/**
 * Convert a basic tile to a formatted tile with the given chrdef
 */
byte * to_formatted_chr(chrdef const & chrdef, byte const * basic_chr);

/**
 * Convert a basic tile to a formatted tile with the given chrdef
 */
void to_formatted_chr_fast(chrdef const & chrdef, byte const * in_basic_chr,
												byte * out_formatted_chr);

/**
 * Convert a formatted tile to a basic tile with the given chrdef
 */
byte * to_basic_chr(chrdef const & chrdef, byte const * formatted_chr);

/**
 * Convert a formatted tile to a basic tile with the given chrdef
 */
void to_basic_chr_fast(chrdef const & chrdef, byte const * in_formatted_chr,
										byte * out_basic_chr);

} // namespace chrgfx

#endif