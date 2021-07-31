#ifndef CHRGFX__CONV_CHR_H
#define CHRGFX__CONV_CHR_H

#include "types.hpp"
#include "chrdef.hpp"

namespace chrgfx
{
// forward declaration of gfxdef types
class buffer;

namespace converters
{
/**
 * Convert a tile to the specified encoding
 */
buffer toFormattedChr(chrdef const & to_def, buffer const & data);

/**
 * Convert a tile from the specified encoding
 */
buffer toBasicChr(chrdef const & from_def, buffer const & data);

} // namespace converters
} // namespace chrgfx

#endif