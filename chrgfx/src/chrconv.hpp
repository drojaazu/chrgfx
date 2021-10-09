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
 * Convert a tile to the specified encoding
 */
char * toFormattedChr(chrdef const & to_def, char const * in_data);

/**
 * Convert a tile from the specified encoding
 */
char * toBasicChr(chrdef const & from_def, char const * in_data);

} // namespace chrgfx

#endif