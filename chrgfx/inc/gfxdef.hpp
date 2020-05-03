#ifndef LIBCHRGFX_GFXDEF_H
#define LIBCHRGFX_GFXDEF_H

#include "types.hpp"

using std::string;

namespace chrgfx
{

class gfx_def
{
public:
	const string &get_id();

protected:
	gfx_def(std::string const &id);
};

class chr_def : public gfx_def
{
public:
	chr_def(string const &id, u16 const width, u16 const height,
					u8 const bitplanes, std::vector<u32> const &planeoffset,
					std::vector<u32> const &pixeloffset,
					std::vector<u32> const &rowoffset);

	u16 get_width();
	u16 get_height();
	u8 get_bitplanes();
	u32 *get_planeoffset();
	u32 *get_pixeloffset();
	u32 *get_rowoffset();
	u32 get_pixeloffset_at(size_t pos);
	u32 get_rowoffset_at(size_t pos);
	u32 get_planeoffset_at(size_t pos);
	u32 get_datasize();
};

class pal_def : public gfx_def
{
public:
	pal_def(string const &id, u8 const entry_datasize, u16 const subpal_length,
					u16 const subpal_count, u8 const subpal_datasize = 0);

	u8 get_entry_datasize();

	u16 get_subpal_length();

	u16 get_subpal_count();

	u8 get_subpal_datasize();
};

class col_def : public gfx_def
{
public:
	col_def(const string id, const std::vector<rgb_layout> layout,
					bool const is_big_endian = false);

	col_def(const string id, const palette &refpal,
					bool const is_big_endian = false);

	rgb_layout get_rgb_pass(size_t pass);

	std::vector<rgb_layout> get_rgb_layout();

	color get_pal_idx(size_t index);

	bool get_is_big_endian();

	bool use_refpal();
};

class rgb_layout
{

public:
	rgb_layout(std::pair<s8, u8> red, std::pair<s8, u8> green,
						 std::pair<s8, u8> blue);

	s8 get_red_shift();
	u8 get_red_count();
	s8 get_green_shift();
	u8 get_green_count();
	s8 get_blue_shift();
	u8 get_blue_count();
};

} // namespace chrgfx

#endif