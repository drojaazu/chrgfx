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

/**
 * Determins the endianness of the local system
 */
inline endianness get_system_endianness()
{
	// shamelessly stolen from stack overflow
	// https://stackoverflow.com/questions/1001307/detecting-endianness-programmatically-in-a-c-program/56191401#56191401
	const int value{0x01};
	const void *address = static_cast<const void *>(&value);
	const unsigned char *least_significant_address =
			static_cast<const unsigned char *>(address);
	return (*least_significant_address == 0x01) ? endianness::little
																							: endianness::big;
}

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
	u8 passes;
	u8 red_shift[MAX_PASS];
	u8 red_bitcount[MAX_PASS];
	u8 green_shift[MAX_PASS];
	u8 green_bitcount[MAX_PASS];
	u8 blue_shift[MAX_PASS];
	u8 blue_bitcount[MAX_PASS];
};

/**
 * System palette definiton
 */
struct pal_def
{
	pal_def(palette (*decoder)(const pal_def *, const u8 *data),
					u8 entry_datasize = 0, u8 subpal_length = 0, u8 subpal_count = 0,
					const color_def *colordef = nullptr, const palette *syspal = nullptr,
					endianness byteorder = endianness::big)
			: decoder(decoder),
				entry_datasize(entry_datasize),
				subpal_length(subpal_length),
				subpal_count(subpal_count),
				colordef(colordef),
				syspal(syspal),
				byteorder(byteorder)
	{
	}
	/**
	 * Pointer to the decoding method
	 */
	palette (*decoder)(const pal_def *, const u8 *data);

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
	 * Pointer to the color definition (for calculated palettes)
	 */
	const color_def *colordef;

	/**
	 * Pointer to system palette (for fixed palettes)
	 */
	const palette *syspal;

	/**
	 * Specify the endianness of the data in the palette
	 */
	endianness byteorder;
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