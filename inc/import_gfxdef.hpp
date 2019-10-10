#ifndef IMPORTGFXDEF_H
#define IMPORTGFXDEF_H
#include <array>
#include <fstream>
#include <map>
#include "global.hpp"

namespace chrgfx
{
namespace defopts
{
static const std::string CHR_WIDTH = "width", CHR_HEIGHT = "height",
												 CHR_BPP = "bpp", CHR_PLANEOFFSET = "planeoffset",
												 CHR_XOFFSET = "xoffset", CHR_YOFFSET = "yoffset";

static const std::string PAL_COLORSIZE = "colorsize",
												 PAL_COLOR_PASSES = "color_passes",
												 PAL_SUBPAL_LENGTH = "subpal_length",
												 PAL_SUBPAL_COUNT = "subpal_count",
												 PAL_SYSPAL = "syspal", PAL_BIG_ENDIAN = "big_endian",
												 PAL_RED_SHIFT = "red_shift",
												 PAL_RED_BITCOUNT = "red_bitcount",
												 PAL_GREEN_SHIFT = "green_shift",
												 PAL_GREEN_BITCOUNT = "green_bitcount",
												 PAL_BLUE_SHIFT = "blue_shift",
												 PAL_BLUE_BITCOUNT = "blue_bitcount";

}	// namespace defopts

void parse_gfxdef(std::ifstream* infile, const chr_def* chrdef,
									const pal_def* paldef);
}	// namespace chrgfx

#endif