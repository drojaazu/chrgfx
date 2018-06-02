#ifndef GFX_TYPES_H
#define GFX_TYPES_H

#include <png++/png.hpp>

#define BYTE uint8_t
#define WORD uint16_t
#define DWORD uint32_t

using namespace png;

namespace gfx
{
typedef pixel_buffer<index_pixel> chr;
typedef std::vector<const chr*> bank;

struct chr_traits
{
	BYTE width;
	BYTE height;
	BYTE bpp;
	WORD data_size;	// calculated as (tile_height * tile_width) / (8 / tile_bpp)
};

struct pal_traits
{
	BYTE palette_length;	// number of colors in a palette
	BYTE color_size;			// size of each color in bytes
};

/**
 * @brief Abstract class for CHR transformers
 *
 */
class chr_xform
{
 public:
	virtual const chr_traits* get_traits() = 0;
	virtual const chr* get_chr(BYTE* data) = 0;
};

/**
 * @brief Abstract class for PAL transformers
 *
 */
class pal_xform
{
 public:
	virtual const pal_traits* get_traits() = 0;
	virtual const color* get_rgb(BYTE* data) = 0;
	virtual const palette* get_pal(BYTE* data) = 0;
};

}	// namespace gfx

#endif