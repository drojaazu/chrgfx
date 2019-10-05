#ifndef GFX_TYPES_H
#define GFX_TYPES_H

#include <array>
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
/*
constants for array sizes
these values are more or less arbitrary and can be set to anything allowed by
the data type though they probably shouldn't need to be)
*/
// Maximum number of color bitplanes allowed
// (since we use 8 bits for a color in normal RGB space, this shouldn't ever
// need to be higher)
constexpr u8 MAX_GFX_PLANES = 4;
// Maximum pixel size of a single chr
constexpr u16 MAX_GFX_SIZE = 8;
// Maximum number of color conversion passes
constexpr u8 MAX_COLOR_PASSES = 16;
// Maximum number of colors in a palette
// (PNG supports up to 256 entries in a palette, so that's our max)
constexpr u16 MAX_COLORS = 256;

/**
 * This defines the format of a chr by describing the offsets of pixels and
 * bitplanes and the size of the tile
 */
class chr_def
{
 public:
	chr_def(u16 width, u16 height, u8 bitplanes,
					std::array<u32, MAX_GFX_PLANES> planeoffset,
					std::array<u32, MAX_GFX_SIZE> xoffset,
					std::array<u32, MAX_GFX_SIZE> yoffset,
					const u8 *(*converter)(const chr_def *chrdef, const u8 *data))
			: width(width),
				height(height),
				bitplanes(bitplanes),
				planeoffset(planeoffset),
				xoffset(xoffset),
				yoffset(yoffset),
				converter(converter),
				datasize(width * height * bitplanes){};

	chr_def &operator=(const chr_def &chrdef)
	{
		if(&chrdef == this) return *this;
		auto out = new chr_def(chrdef);
		return *out;
	};

	const u8 *convert(const u8 *data) { return this->converter(this, data); }

	const u16 width;		 // pixel width of each element
	const u16 height;		 // pixel height of each element
	const u8 bitplanes;	// number of color bitplanes
	const std::array<u32, MAX_GFX_PLANES>
			planeoffset;	// bit offset of each bitplane
	const std::array<u32, MAX_GFX_SIZE>
			xoffset;	// bit offset of each horizontal pixel
	const std::array<u32, MAX_GFX_SIZE>
			yoffset;	// bit offset of each vertical pixel
	const u8 *(*converter)(const chr_def *, const u8 *);
	const u32 datasize;	// size of one chr in bits
};

/**
 * This defines the format of color data by describing the offset and number of
 * the red, green and blue component bits within the data class color_def
 */
class color_def
{
 public:
	const u8 color_passes;
	const std::array<u8, MAX_COLOR_PASSES> red_shift;
	const std::array<u8, MAX_COLOR_PASSES> red_bitcount;
	const std::array<u8, MAX_COLOR_PASSES> green_shift;
	const std::array<u8, MAX_COLOR_PASSES> green_bitcount;
	const std::array<u8, MAX_COLOR_PASSES> blue_shift;
	const std::array<u8, MAX_COLOR_PASSES> blue_bitcount;
};

/**
 * This defines the format of a color palette by specifying the size of the full
 * palette and subpalettes, a color definition or a fixed system palette, and
 * the endianness of the data
 */
class pal_def
{
 public:
	pal_def &operator=(const pal_def &paldef)
	{
		if(&paldef == this) return *this;
		auto out = new pal_def(paldef);
		return *out;
	}
	/**
	 * The size of each palette entry in bits
	 */
	const u8 entry_datasize;

	/**
	 * The number entries in a subpalette
	 */
	const u8 subpal_length;

	/**
	 * The number of subpalettes in a full palette
	 */
	const u8 subpal_count;

	/**
	 * Pointer to the color definition (for calculated palettes)
	 */
	const color_def *colordef;

	/**
	 * Pointer to system palette (for fixed palettes)
	 */
	const palette *syspal;

	/**
	 * Pointer to the decoding method
	 */
	const palette *(*converter)(const pal_def *paldef, const u8 *data,
															const s16 subpal_idx);

	/**
	 * Specify the endianness of the data in the palette
	 */
	const bool is_big_endian;

	const palette *convert(const u8 *data, const s16 subpal_idx)
	{
		return this->converter(this, data, subpal_idx);
	};
};

/*
 * A collection of CHRs of the same type, with supporting attributes
 */
class bank
{
 public:
	bank(const chr_def *chrdef) : chrdef(chrdef)
	{
		this->chrs = new std::vector<const u8 *>();
	};

	~bank()
	{
		for(const u8 *this_chr : *(this->chrs)) delete[] this_chr;
		delete this->chrs;
	};

	const chr_def *chrdef;
	std::vector<const u8 *> *chrs;
};

}	// namespace chrgfx

#endif