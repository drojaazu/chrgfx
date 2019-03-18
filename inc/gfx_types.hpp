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

/*!
 * \brief CHR attributes
 */
struct chr_traits
{
	const u8 width;		//!< Width in pixels
	const u8 height;	//!< Height in pixels
	const u8 bpp;			//!< Color bits per pixel
	const u16
			data_size;	//!< Calculated as (tile_height * tile_width) / (8 / tile_bpp)
};

/*!
 * \brief Palette attributes
 */
struct pal_traits
{
	const u16 palette_length;	//!< Number of colors in the palette
	const u8 color_size;			 //!< Data size of each color in bytes
	const int8_t
			subpalette_count;	//!< Number of subpalettes within the whole palette
	const u8 subpalette_length;	//<<! Number of colors in a subpalette
};

/*!
 * \brief Abstract class for CHR transformers
 */
class chr_xform
{
 public:
	virtual const chr_traits *get_traits() = 0;
	virtual const chr *get_chr(const u8 *data) = 0;
	virtual ~chr_xform(){};
};

/*!
 * \brief Abstract class for palette transformers
 */
class pal_xform
{
 public:
	virtual const pal_traits *get_traits() = 0;
	virtual const color *get_rgb(const u8 *data) = 0;
	virtual const palette *get_pal(const u8 *data, int8_t subpalette) = 0;
	virtual ~pal_xform(){};
};

/*!
 * \brief A collection of CHRs of the same type, with supporting attributes
 */
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