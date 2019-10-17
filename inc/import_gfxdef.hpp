#ifndef IMPORTGFXDEF_H
#define IMPORTGFXDEF_H
#include <bits/stdc++.h>

#include <array>
#include <fstream>
#include <map>
#include <sstream>

#include "chrdecode.hpp"
#include "global.hpp"
#include "paldecode.hpp"

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

}	 // namespace defopts

void parse_gfxdef(std::ifstream* infile, const chr_def* chrdef,
									const pal_def* paldef);

void validate_chrdef_opts(std::map<const std::string, std::string>* chrdef_opts,
													const chr_def* chrdef);

void validate_paldef_opts(std::map<const std::string, std::string>* paldef_opts,
													const pal_def* paldef);

u32 str_validate_ispos(std::string value);

u32 str_validate_nonneg(std::string value);

std::vector<u32> str_validate_array_vals_nonneg(std::string value);

std::vector<u32> str_validate_array_vals_ispos_length(std::string value,
																											s32 req_length);

palette* str_validate_array_get_palette(std::string value);
}	 // namespace chrgfx

#endif