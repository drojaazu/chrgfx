#ifndef CHRGFX__GFXDEF_H
#define CHRGFX__GFXDEF_H

#include "types.hpp"
#include <png++/png.hpp>
#include <string>
#include <vector>

using png::color;
using png::palette;
using std::pair;
using std::string;
using std::vector;

namespace chrgfx
{

class rgb_layout
{
public:
	rgb_layout(pair<s8, u8> red, pair<s8, u8> green, pair<s8, u8> blue);

	s8 get_red_shift() const;
	u8 get_red_count() const;
	s8 get_green_shift() const;
	u8 get_green_count() const;
	s8 get_blue_shift() const;
	u8 get_blue_count() const;

private:
	pair<s8, u8> red;
	pair<s8, u8> green;
	pair<s8, u8> blue;
};

class gfxdef
{
public:
	string get_id() const;

protected:
	gfxdef(string const &id);

private:
	string id;
};

class chrdef : public gfxdef
{
public:
	chrdef(string const &id, u16 const width, u16 const height,
				 u8 const bitplanes, vector<u32> const &planeoffset,
				 vector<u32> const &pixeloffset, vector<u32> const &rowoffset);

	u16 get_width() const;
	u16 get_height() const;
	u8 get_bitplanes() const;

	u32 const *get_planeoffset() const;
	u32 const *get_pixeloffset() const;
	u32 const *get_rowoffset() const;
	u32 get_pixeloffset_at(size_t pos) const;
	u32 get_rowoffset_at(size_t pos) const;
	u32 get_planeoffset_at(size_t pos) const;

	u32 get_datasize() const;

private:
	u16 width;
	u16 height;
	u8 bitplanes;

	vector<u32> planeoffset;
	u32 const *planeoffset_data;

	vector<u32> pixeloffset;
	u32 const *pixeloffset_data;

	vector<u32> rowoffset;
	u32 const *rowoffset_data;

	u32 datasize; // size of one chr in bits
};

/**
 * Describes color data by specifying the offset and data size of each
 * component (red, green, blue) within the data
 */
class coldef : public gfxdef
{
public:
	coldef(const string id, const vector<rgb_layout> layout,
				 bool is_big_endian = false);

	coldef(const string id, const palette &refpal, bool is_big_endian = false);

	rgb_layout get_rgb_pass(size_t pass) const;

	vector<rgb_layout> get_rgb_layout() const;

	color get_pal_idx(size_t index) const;

	bool use_refpal() const;
	bool get_is_big_endian() const;

private:
	vector<rgb_layout> layout;
	palette refpal;
	const bool is_refpal;

	/**
	 * Specify the endianness of the color data
	 */
	const bool is_big_endian;
};

/**
 * Defines the format of a color palette by specifying the size and count of
 * subpalettes, as well as either a color definition or pre-defined color set
 */
class paldef : public gfxdef
{
public:
	paldef(string const &id, u8 const entry_datasize, u16 const subpal_length,
				 u16 const subpal_count, u8 const subpal_datasize = 0);

	u8 get_entry_datasize() const;

	u16 get_subpal_length() const;

	u16 get_subpal_count() const;

	u8 get_subpal_datasize() const;

private:
	u8 entry_datasize;
	u16 subpal_length;
	u16 subpal_count;
	u8 subpal_datasize;
};

} // namespace chrgfx

#endif
