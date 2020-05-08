#ifndef LIBCHRGFX_GFXDEF_H
#define LIBCHRGFX_GFXDEF_H

#include "types.hpp"
#include <png++/png.hpp>

using namespace png;

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
};

class gfx_def
{
public:
	string get_id() const;

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

	u16 get_width() const;
	u16 get_height() const;
	u8 get_bitplanes() const;
	u32 *get_planeoffset() const;
	u32 *get_pixeloffset() const;
	u32 *get_rowoffset() const;
	u32 get_pixeloffset_at(size_t pos) const;
	u32 get_rowoffset_at(size_t pos) const;
	u32 get_planeoffset_at(size_t pos) const;
	u32 get_datasize() const;
};

class pal_def : public gfx_def
{
public:
	pal_def(string const &id, u8 const entry_datasize, u16 const subpal_length,
					u16 const subpal_count, u8 const subpal_datasize = 0);

	u8 get_entry_datasize() const;

	u16 get_subpal_length() const;

	u16 get_subpal_count() const;

	u8 get_subpal_datasize() const;
};

class col_def : public gfx_def
{
public:
	col_def(const string id, const std::vector<rgb_layout> layout,
					bool const is_big_endian = false);

	col_def(const string id, const palette &refpal,
					bool const is_big_endian = false);

	rgb_layout get_rgb_pass(size_t pass) const;

	std::vector<rgb_layout> get_rgb_layout() const;

	color get_pal_idx(size_t index) const;

	bool get_is_big_endian() const;

	bool use_refpal() const;
};

} // namespace chrgfx

#endif
