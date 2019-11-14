#ifndef GFX_TYPES_H
#define GFX_TYPES_H

#include <array>
#include <map>
#include <memory>
#include <png++/png.hpp>

using namespace png;

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;

typedef int8_t s8;
typedef int16_t s16;
typedef int32_t s32;

template <typename T> using uptr = std::unique_ptr<T>;
template <typename T> using sptr = std::shared_ptr<T>;

/**
 * Key-value map
 */
typedef std::map<const std::string, std::string> kvmap;

/**
 * Pointer to a block of immutable byte data
 */
typedef uint8_t const *bptr;

namespace chrgfx
{
/*
	Constants for allocation of fixed array sizes

		The values below are more or less arbitrary
		and can be set to anything allowed by the data type
		(though they really shouldn't need to be)
*/

/**
 * Maximum number of bitplanes
 * (Since we use 8 bits for a color in standard RGB space, this shouldn't ever
 * need to be higher)
 */
constexpr u8 MAX_CHR_PLANES = 8;

/**
 * Maximum dimensions of a single chr (in pixels)
 */
constexpr u16 MAX_CHR_SIZE = 64;

/**
 * Maximum number of color conversion passes
 */
constexpr u8 MAX_COLOR_PASSES = 16;

/**
 * Describes a graphic tile by specifying dimensions and bitplane and pixel
 * offsets
 */
class chr_def
{
public:
	chr_def();

	chr_def(u16 width, u16 height, u8 bitplanes,
					std::array<u32, MAX_CHR_PLANES> planeoffset,
					std::array<u32, MAX_CHR_SIZE> xoffset,
					std::array<u32, MAX_CHR_SIZE> yoffset,
					bptr (*converter)(chr_def &chrdef, bptr data))
			: width(width), height(height), bitplanes(bitplanes),
				planeoffset(planeoffset), xoffset(xoffset), yoffset(yoffset),
				converter(converter), datasize(width * height * bitplanes){};

	u8 const *convert(u8 const *data) { return converter(*this, data); }

	u16 get_width() { return width; }

	u16 get_height() { return height; }

	u8 get_bitplanes() { return bitplanes; }

	std::array<u32, MAX_CHR_PLANES> get_planeoffset() { return planeoffset; };
	std::array<u32, MAX_CHR_SIZE> get_xoffset() { return xoffset; };
	std::array<u32, MAX_CHR_SIZE> get_yoffset() { return yoffset; };

	u32 get_datasize() { return datasize; }

private:
	u16 width;
	u16 height;
	u8 bitplanes;
	std::array<u32, MAX_CHR_PLANES> planeoffset;
	std::array<u32, MAX_CHR_SIZE> xoffset;
	std::array<u32, MAX_CHR_SIZE> yoffset;
	bptr (*converter)(chr_def &, bptr);

	u32 datasize; // size of one chr in bits
};

/**
 * Describes color data by specifying the offset and data size of each component
 * (red, green, blue) within the data
 */
class col_def
{
public:
	col_def(u8 color_passes, std::array<u8, MAX_COLOR_PASSES> red_shift,
					std::array<u8, MAX_COLOR_PASSES> red_bitcount,
					std::array<u8, MAX_COLOR_PASSES> green_shift,
					std::array<u8, MAX_COLOR_PASSES> green_bitcount,
					std::array<u8, MAX_COLOR_PASSES> blue_shift,
					std::array<u8, MAX_COLOR_PASSES> blue_bitcount)
			: color_passes(color_passes), red_shift(red_shift),
				red_bitcount(red_bitcount), green_shift(green_shift),
				green_bitcount(green_bitcount), blue_shift(blue_shift),
				blue_bitcount(blue_bitcount){};

	// return by value and let the compiler deal with RVO
	u8 get_color_passes() { return color_passes; }

	std::array<u8, MAX_COLOR_PASSES> get_red_shift() { return red_shift; };
	std::array<u8, MAX_COLOR_PASSES> get_red_bitcount() { return red_bitcount; };
	std::array<u8, MAX_COLOR_PASSES> get_green_shift() { return green_shift; };
	std::array<u8, MAX_COLOR_PASSES> get_green_bitcount()
	{
		return green_bitcount;
	};
	std::array<u8, MAX_COLOR_PASSES> get_blue_shift() { return blue_shift; };
	std::array<u8, MAX_COLOR_PASSES> get_blue_bitcount()
	{
		return blue_bitcount;
	};

private:
	u8 color_passes;
	std::array<u8, MAX_COLOR_PASSES> red_shift;
	std::array<u8, MAX_COLOR_PASSES> red_bitcount;
	std::array<u8, MAX_COLOR_PASSES> green_shift;
	std::array<u8, MAX_COLOR_PASSES> green_bitcount;
	std::array<u8, MAX_COLOR_PASSES> blue_shift;
	std::array<u8, MAX_COLOR_PASSES> blue_bitcount;
};

/**
 * Defines the format of a color palette by specifying the size and count of
 * subpalettes, as well as either a color definition or pre-defined color set
 */
class pal_def
{
public:
	pal_def();

	pal_def(u8 entry_datasize, u8 subpal_length, u8 subpal_count, col_def *coldef,
					palette *syspal,
					palette *(*converter)(pal_def &paldef, bptr data, s16 subpal_idx),
					bool is_big_endian = false, u8 subpal_datasize = 0)
			: entry_datasize(entry_datasize), subpal_length(subpal_length),
				subpal_count(subpal_count), coldef(coldef), syspal(syspal),
				converter(converter), is_big_endian(is_big_endian),
				subpal_datasize(subpal_datasize){};

	u8 get_entry_datasize() { return entry_datasize; }

	u8 get_subpal_length() { return subpal_length; }

	u8 get_subpal_count() { return subpal_count; }

	u8 get_subpal_datasize() { return subpal_datasize; }

	col_def *get_coldef() { return coldef; }

	palette *get_syspal() { return syspal; }

	bool get_is_big_endian() { return is_big_endian; }

	palette *convert(u8 const *data, s16 subpal_idx)
	{
		return converter(*this, data, subpal_idx);
	}

private:
	/**
	 * The size of each color entry in bits
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

	u8 subpal_datasize;

	/**
	 * Pointer to the color definition (for calculated palettes)
	 */
	col_def *coldef;

	/**
	 * Pointer to system palette (for fixed palettes)
	 */
	palette *syspal;

	/**
	 * Pointer to the decoding method
	 */
	palette *(*converter)(pal_def &paldef, bptr data, s16 subpal_idx);

	/**
	 * Specify the endianness of the data in the palette
	 */
	const bool is_big_endian;
};

/**
 * A collection of tiles of the same type, with supporting attributes
 */
class bank
{
public:
	bank(chr_def chrdef) : chrdef(chrdef)
	{
		// the tile vector might get big, so let's keep it on the heap
		this->chrs = new std::vector<bptr>();
	};

	~bank()
	{
		for(bptr this_chr : *(this->chrs))
			delete[] this_chr;
		delete this->chrs;
	};

	chr_def chrdef;
	std::vector<bptr> *chrs;
};

} // namespace chrgfx

#endif