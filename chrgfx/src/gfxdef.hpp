#ifndef CHRGFX__GFXDEF_H
#define CHRGFX__GFXDEF_H

#include "types.hpp"
//#include <map>
#include <png++/png.hpp>
#include <string>

using png::color;
using png::palette;
using std::string;

namespace chrgfx
{

class rgb_layout
{

public:
	rgb_layout(std::pair<s8, u8> red, std::pair<s8, u8> green,
						 std::pair<s8, u8> blue);

	s8 get_red_shift() const;
	u8 get_red_count() const;
	s8 get_green_shift() const;
	u8 get_green_count() const;
	s8 get_blue_shift() const;
	u8 get_blue_count() const;

private:
	std::pair<s8, u8> red;
	std::pair<s8, u8> green;
	std::pair<s8, u8> blue;
};

class gfx_def
{
public:
	string get_id() const;

protected:
	gfx_def(string const &id);

private:
	string id;
};

class chr_def : public gfx_def
{
public:
	chr_def(std::string const &id, u16 const width, u16 const height,
					u8 const bitplanes, std::vector<u32> const &planeoffset,
					std::vector<u32> const &pixeloffset,
					std::vector<u32> const &rowoffset);

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

	std::vector<u32> planeoffset;
	u32 const *planeoffset_data;

	std::vector<u32> pixeloffset;
	u32 const *pixeloffset_data;

	std::vector<u32> rowoffset;
	u32 const *rowoffset_data;

	u32 datasize; // size of one chr in bits
};

/**
 * Describes color data by specifying the offset and data size of each
 * component (red, green, blue) within the data
 */
class col_def : public gfx_def
{
public:
	col_def(const std::string id, const std::vector<rgb_layout> layout,
					bool is_big_endian = false);

	col_def(const std::string id, const palette &refpal,
					bool is_big_endian = false);

	rgb_layout get_rgb_pass(size_t pass) const;

	std::vector<rgb_layout> get_rgb_layout() const;

	color get_pal_idx(size_t index) const;

	bool use_refpal() const;
	bool get_is_big_endian() const;

private:
	std::vector<rgb_layout> layout;
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
class pal_def : public gfx_def
{
public:
	pal_def(std::string const &id, u8 const entry_datasize,
					u16 const subpal_length, u16 const subpal_count,
					u8 const subpal_datasize = 0);

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
