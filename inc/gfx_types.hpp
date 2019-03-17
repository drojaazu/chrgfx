#ifndef GFX_TYPES_H
#define GFX_TYPES_H

#include <png++/png.hpp>

using namespace png;

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;

namespace chrgfx
{
typedef u8 chr;

struct chr_traits
{
	const u8 width;
	const u8 height;
	const u8 bpp;
	const u16
			data_size;	// calculated as (tile_height * tile_width) / (8 / tile_bpp)
};

struct pal_traits
{
	const u16 palette_length;	// number of colors in a palette
	const u8 color_size;			 // size of each color in bytes
	const int8_t
			subpalette_count;				 // number of subpalettes within the whole palette
	const u8 subpalette_length;	// number of colors in a subpalette
};

/**
 * @brief Abstract class for CHR transformers
 *
 */
class chr_xform
{
 public:
	virtual const chr_traits *get_traits() = 0;
	virtual const chr *get_chr(u8 *data) = 0;
	virtual ~chr_xform(){};
};

/**
 * @brief Abstract class for PAL transformers
 *
 */
class pal_xform
{
 public:
	virtual const pal_traits *get_traits() = 0;
	virtual const color *get_rgb(u8 *data) = 0;
	virtual const palette *get_pal(u8 *data, int8_t subpalette) = 0;
	virtual ~pal_xform(){};
};

// const pal_traits pal_xform::traits{0, 0, 0, 0};

class bank
{
	const chr_traits c_traits;
	std::vector<const chr *> *c_data = nullptr;

 public:
	bank(const chr_traits traits) : c_traits(traits)
	{
		this->c_data = new std::vector<const chr *>();
	};

	~bank()
	{
		for(const chr *this_chr : *(this->c_data)) delete[] this_chr;
		delete this->c_data;
	};

	const chr_traits traits() { return this->c_traits; }
	std::vector<const chr *> *data() { return this->c_data; }
};

}	// namespace chrgfx

#endif