#ifndef COMMONDEFS_H
#define COMMONDEFS_H

#include "chr_conv.hpp"
#include "pal_conv.hpp"
#include "types.hpp"

namespace chrgfx
{
/*
	CUSTOM/NON-STANDARD/INTERNAL GRAPHICS DEFINITIONS:
	Any gfxdefs you wish to be compiled internally need to be defined here, in
	either the chrdefs or paldefs namespace. The code for non-standard conversion
	routines belongs in chr_conv or pal_conv.
*/
namespace chrdefs
{
/*
		CHRDEF INITIALIZER:
		{
			width,
			height,
			bitplanes,
			{bitplane0,bitplane1...},
			{xoffset0,xoffset1...},
			{yoffset0,y0ffset1...},
			conversion function
		}
*/

// ----- FALLBACK FORMAT
static chr_def chr_8x8x1{
		8,
		8,
		1,
		{0},
		{0, 1, 2, 3, 4, 5, 6, 7},
		{0 * 8, 1 * 8, 2 * 8, 3 * 8, 4 * 8, 5 * 8, 6 * 8, 7 * 8},
		get_chr};

// ----- NON-STANDARD CONVERTERS
// TODO: Nintendo SFC 3bpp tiles

} // namespace chrdefs

namespace paldefs
{
// ----- NON-STANDARD CONVERTERS

/*
		PALDEF INITIALIZER:
		{
			colorsize,
			subpal_length,
			subpal_count,
			*coldef,
			*refpalm,
			conversion function,
			is_bigendian = false,
			subpal_datasize = 0
		}
*/

/*
		COLDEF INITIALIZER:
		{
			passes,
			{red_shift0,...},
			{red_size0,...}
			{green_shift0,...},
			{green_size0,...},
			{blue_shift0,...},
			{blue_size0,...}
		}
*/

/**
 * TileLayer Pro palette format
 */
static pal_def tilelayerpro{24, 255, 1, nullptr, nullptr, &pal_decode_soft_tlp};

} // namespace paldefs

/*
		Any custom def's created must be added to this map!
		The key is the identifier used on the command line to reference the format
*/
std::map<std::string, chr_def> internal_chrdefs = {
		{std::string("mono"), chrdefs::chr_8x8x1}};

std::map<std::string, pal_def> internal_paldefs = {
		{std::string("tlp"), paldefs::tilelayerpro}};

} // namespace chrgfx
#endif