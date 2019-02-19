#ifndef GFX_TYPES_H
#define GFX_TYPES_H

#include <png++/png.hpp>

using namespace png;

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;

namespace gfx
{
typedef pixel_buffer<index_pixel> chr;
typedef std::vector<const chr*> bank;

struct chr_traits
{
	u8 width;
	u8 height;
	u8 bpp;
	u16 data_size;	// calculated as (tile_height * tile_width) / (8 / tile_bpp)
};

struct pal_traits
{
	u8 palette_length;	// number of colors in a palette
	u8 color_size;			// size of each color in bytes
};

/**
 * @brief Abstract class for CHR transformers
 *
 */
class chr_xform
{
 public:
	virtual const chr_traits* get_traits() = 0;
	virtual const chr* get_chr(u8* data) = 0;
};

/**
 * @brief Abstract class for PAL transformers
 *
 */
class pal_xform
{
 public:
	virtual const pal_traits* get_traits() = 0;
	virtual const color* get_rgb(u8* data) = 0;
	virtual const palette* get_pal(u8* data) = 0;
};

}	// namespace gfx

#endif