#ifndef CHRGFX__CONV_CHR_H
#define CHRGFX__CONV_CHR_H

#include "types.hpp"
#include <iomanip>
#include <map>
#include <string>

namespace chrgfx
{
// forward declaration of gfxdef types
class chrdef;
class coldef;
class paldef;

namespace conv_chr
{
/**
 * Represents a function to convert a tile to a given encoding
 */
typedef u8 *(*cvto_chr_t)(chrdef const &, u8 const *);

/**
 * Represents a function to convert a tile from a given encoding
 */
typedef u8 *(*cvfrom_chr_t)(chrdef const &, u8 const *);

/**
 * Reference of all functions for converting a tile to a given encoding
 */
extern std::map<std::string const, cvto_chr_t> const converters_to;

/**
 * Reference of all functions for converting a tile from a given encoding
 */
extern std::map<std::string const, cvfrom_chr_t> const converters_from;

/**
 * Convert a tile to the specified encoding
 */
u8 *cvto_chr(chrdef const &to_def, u8 const *data);

/**
 * Convert a tile from the specified encoding
 */
u8 *cvfrom_chr(chrdef const &from_def, u8 const *data);

} // namespace conv_chr
} // namespace chrgfx

#endif