#include "gfxdef.hpp"

namespace chrgfx
{

gfx_def::gfx_def(string const &id) : id(id){};

string gfx_def::get_id() const
{
#ifdef DEBUG
	std::cerr << "New gfxdef: " << id << std::endl;
#endif
	return id;
};

rgb_layout::rgb_layout(std::pair<s8, u8> red, std::pair<s8, u8> green,
											 std::pair<s8, u8> blue)
		: red(red), green(green), blue(blue){};

s8 rgb_layout::get_red_shift() const { return red.first; }
u8 rgb_layout::get_red_count() const { return red.second; }
s8 rgb_layout::get_green_shift() const { return green.first; }
u8 rgb_layout::get_green_count() const { return green.second; }
s8 rgb_layout::get_blue_shift() const { return blue.first; }
u8 rgb_layout::get_blue_count() const { return blue.second; }

chr_def::chr_def(std::string const &id, u16 const width, u16 const height,
								 u8 const bitplanes, std::vector<u32> const &planeoffset,
								 std::vector<u32> const &pixeloffset,
								 std::vector<u32> const &rowoffset)
		: gfx_def(std::move(id)), width(std::move(width)),
			height(std::move(height)), bitplanes(std::move(bitplanes)),
			planeoffset(std::move(planeoffset)), pixeloffset(std::move(pixeloffset)),
			rowoffset(std::move(rowoffset)), datasize(width * height * bitplanes)
{
	// putting these here instead of the initializer list
	// to guarantee they point to the right place after any moves
	planeoffset_data = this->planeoffset.data();
	pixeloffset_data = this->pixeloffset.data();
	rowoffset_data = this->rowoffset.data();
}

u16 chr_def::get_width() const { return width; }
u16 chr_def::get_height() const { return height; }
u8 chr_def::get_bitplanes() const { return bitplanes; }
u32 const *chr_def::get_planeoffset() const { return planeoffset.data(); };
u32 const *chr_def::get_pixeloffset() const { return pixeloffset.data(); };
u32 const *chr_def::get_rowoffset() const { return rowoffset.data(); };
u32 chr_def::get_pixeloffset_at(size_t pos) const { return pixeloffset[pos]; };
u32 chr_def::get_rowoffset_at(size_t pos) const { return rowoffset[pos]; };
u32 chr_def::get_planeoffset_at(size_t pos) const { return planeoffset[pos]; };
u32 chr_def::get_datasize() const { return datasize; }

col_def::col_def(const std::string id, const std::vector<rgb_layout> layout,
								 bool is_big_endian)
		: gfx_def(std::move(id)), layout(std::move(layout)),
			is_big_endian(std::move(is_big_endian)), is_refpal(false){};

col_def::col_def(const std::string id, const palette &refpal,
								 bool is_big_endian)
		: gfx_def(std::move(id)), refpal(std::move(refpal)),
			is_big_endian(std::move(is_big_endian)), is_refpal(true){};

rgb_layout col_def::get_rgb_pass(size_t pass) const { return layout.at(pass); }

std::vector<rgb_layout> col_def::get_rgb_layout() const { return layout; }

color col_def::get_pal_idx(size_t index) const { return refpal.at(index); }

bool col_def::use_refpal() const { return is_refpal; }
bool col_def::get_is_big_endian() const { return is_big_endian; }

pal_def::pal_def(std::string const &id, u8 const entry_datasize,
								 u16 const subpal_length, u16 const subpal_count,
								 u8 const subpal_datasize)
		: gfx_def(std::move(id)), entry_datasize(std::move(entry_datasize)),
			subpal_length(std::move(subpal_length)),
			subpal_count(std::move(subpal_count)),
			subpal_datasize(std::move(subpal_datasize)){};

u8 pal_def::get_entry_datasize() const { return entry_datasize; }

u16 pal_def::get_subpal_length() const { return subpal_length; }

u16 pal_def::get_subpal_count() const { return subpal_count; }

u8 pal_def::get_subpal_datasize() const { return subpal_datasize; }
} // namespace chrgfx
