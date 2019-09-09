#ifndef GFX_TYPES_H
#define GFX_TYPES_H

#include <png++/png.hpp>

using namespace png;

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;

typedef int8_t s8;
typedef int16_t s16;
typedef int32_t s32;

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
	const int16_t
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
	virtual const palette *get_pal(const u8 *data, int16_t subpalette) = 0;
	virtual ~pal_xform(){};
};

constexpr u16 MAX_GFX_PLANES = 8;
constexpr u16 MAX_GFX_SIZE = 32;

// This is based on gfx_layout in mame, with the exlusion of a couple of unused
// fields
struct chr_def
{
	u16 width;												// pixel width of each element
	u16 height;												// pixel height of each element
	u16 planes;												// number of bitplanes
	u32 planeoffset[MAX_GFX_PLANES];	// bit offset of each bitplane
	u32 xoffset[MAX_GFX_SIZE];				// bit offset of each horizontal pixel
	u32 yoffset[MAX_GFX_SIZE];				// bit offset of each vertical pixel
	u32 charincrement;	// distance between two consecutive elements (in bits)
};

constexpr u16 MAX_PASS = 16;
constexpr u16 MAX_COLORS = 256;
struct color_def
{
	// color info
	u16 passes;
	u16 red_shift[MAX_PASS];
	u16 red_mask[MAX_PASS];
	u16 green_shift[MAX_PASS];
	u16 green_mask[MAX_PASS];
	u16 blue_shift[MAX_PASS];
	u16 blue_mask[MAX_PASS];
};

/**
 * System palette definiton
 */
struct pal_def
{
	/**
	 * The number of entries in the system palette
	 */
	u16 syspal_size;
	/**
	 * The number entries in a subpalette
	 */
	u16 subpal_size;

	u16 entry_size;	// per color, in bytes

	/**
	 * Pointer to the hard coded system palette, if relevant
	 */
	const color *syspal;

	const color_def *colordef;
};

/*
 * A collection of CHRs of the same type, with supporting attributes
 */
class bank
{
 public:
	bank(const chr_def chrdef) : c_def(chrdef)
	{
		this->c_data = new std::vector<const u8 *>();
	};

	~bank()
	{
		for(const chr *this_chr : *(this->c_data)) delete[] this_chr;
		delete this->c_data;
	};

	const chr_def chrdef() { return this->c_def; }
	std::vector<const u8 *> *data() { return this->c_data; }

 private:
	chr_def c_def;
	std::vector<const u8 *> *c_data;
};

}	// namespace chrgfx

#endif