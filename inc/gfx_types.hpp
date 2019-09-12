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
enum class endianness
{
	little = 0,
	big = 1,
};

// shamelessly stolen from stack overflow
// https://stackoverflow.com/questions/1001307/detecting-endianness-programmatically-in-a-c-program/56191401#56191401
inline endianness get_system_endianness()
{
	const int value{0x01};
	const void *address = static_cast<const void *>(&value);
	const unsigned char *least_significant_address =
			static_cast<const unsigned char *>(address);
	return (*least_significant_address == 0x01) ? endianness::little
																							: endianness::big;
}
// typedef u8 chr;

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
	virtual const u8 *get_chr(const u8 *data) = 0;
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
	endianness byteorder;
};

/**
 * System palette definiton
 */
struct pal_def
{
	pal_def(u8 entry_datasize, u8 subpal_length, u8 subpal_count,
					palette (*decoder)(const pal_def *, const u8 *data),
					const color_def *colordef, const palette *syspal)
			: entry_datasize(entry_datasize),
				subpal_length(subpal_length),
				subpal_count(subpal_count),
				decoder(decoder),
				colordef(colordef),
				syspal(syspal)

	{
	}

	/**
	 * The size of each palette entry in bits
	 */
	u8 entry_datasize;

	/**
	 * The number entries in a subpalette
	 */
	u8 subpal_length;

	/**
	 * The number of subpalettes in a full palette
	 */
	u8 subpal_count;

	/**
	 * Pointer to the decoding method
	 */
	palette (*decoder)(const pal_def *, const u8 *data);

	/**
	 * Pointer to the color definition (for calculated palettes)
	 */
	const color_def *colordef;

	/**
	 * Pointer to system palette (for fixed palettes)
	 */
	const palette *syspal;
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
		for(const u8 *this_chr : *(this->c_data)) delete[] this_chr;
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