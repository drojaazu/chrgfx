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
/**
 * Defines the strings of the tile definition keys
 */
static const std::string
		// x and y dimensions of the tile
		CHR_WIDTH = "width",
		CHR_HEIGHT = "height",
		// bitdepth
		CHR_BPP = "bpp",
		// offset (in bits) of each bitplane
		CHR_PLANEOFFSET = "planeoffset",
		// offset (in bits) of each pixel in a row
		CHR_XOFFSET = "xoffset",
		// offset (in bits) of the start of each row
		CHR_YOFFSET = "yoffset";

/**
 * Defines the strings of the color & palette definition keys
 */
static const std::string PAL_COLORSIZE = "colorsize",
												 PAL_COLOR_PASSES = "color_passes",
												 PAL_SUBPAL_LENGTH = "subpal_length",
												 PAL_SUBPAL_COUNT = "subpal_count",
												 PAL_SUBPAL_DATASIZE = "subpal_datasize",
												 PAL_REFPAL = "refpal", PAL_BIG_ENDIAN = "big_endian",
												 PAL_RED_SHIFT = "red_shift", PAL_RED_SIZE = "red_size",
												 PAL_GREEN_SHIFT = "green_shift",
												 PAL_GREEN_SIZE = "green_size",
												 PAL_BLUE_SHIFT = "blue_shift",
												 PAL_BLUE_SIZE = "blue_size";

} // namespace defopts

/**
 * Returns a chr_def object populated with values from the specified gfxdef file
 */
chr_def *get_chrdef(std::ifstream &infile);

/**
 * Returns a pal_def object populated with values from the specified gfxdef file
 */
pal_def *get_paldef(std::ifstream &infile);

/**
 * Parse the gfxdef text file into valid key-value pairs
 */
kvmap parse_file(std::ifstream &infile);

/**
 * Return true if value is > 0, false if <1
 */
bool str_bool(std::string value);

/**
 * Ensure value is greater than 0
 */
u32 str_validate_ispos(std::string value);

/**
 * Ensure value is greater than or equal to 0
 */
u32 str_validate_nonneg(std::string value);

/**
 * Ensure value is a comma delimited list of values, where each value is greater
 * than or equal to 0
 */
std::vector<u32> str_validate_array_vals_nonneg(std::string value);

/**
 * Ensure value is a comma delimited list of values, where each value is greater
 * than or equal to 0, and the total number of elements must match req_length
 */
std::vector<u32> str_validate_array_vals_nonneg_length(std::string value,
																											 s32 req_length);

palette *str_validate_array_get_palette(std::string value);
} // namespace chrgfx

#endif