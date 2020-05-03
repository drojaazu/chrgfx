#ifndef LIBCHRGFX_TYPEDEFS_H
#define LIBCHRGFX_TYPEDEFS_H

#include "gfxdef.hpp"
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

namespace chrgfx
{
/**
 * Pointer to a block of byte data
 */
typedef uint8_t *chunk;

/*
	typedef chunk so we know, semantically, what kind of
	data we're working with
*/

// a chunk of native (8bit packed pixel) chr data
typedef chunk rawchr;
// a chunk of any non-native chr data (requires a chrdef)
typedef chunk defchr;

struct render_traits {
	u16 cols = 8;
	u8 tile_border = 0;
	bool use_trns = false;
	u8 trns_entry = 0;
};

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

// TODO: convert bank to a typedef of pair<chr_def, vector>
/**
 * A collection of tiles of the same type, with supporting attributes
 */
class bank
{
public:
	bank(chr_def chrdef) : chrdef(chrdef)
	{
		// the tile vector might get big, so let's keep it on the heap
		this->chrs = new std::vector<chunk>();
	};

	~bank()
	{
		for(chunk this_chr : *(this->chrs))
			delete[] this_chr;
		delete this->chrs;
	};

	chr_def chrdef;
	std::vector<chunk> *chrs;
};

// typedef our ugly function pointers
typedef chunk (*chr_cv)(chr_def &, chunk);
typedef palette *(*pal_cv)(pal_def &, chunk, s16);
} // namespace chrgfx

#endif