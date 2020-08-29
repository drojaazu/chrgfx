#include "gfxdef.hpp"

namespace chrgfx
{

gfxdef::gfxdef(string const &id) : id(id)
{
#ifdef DEBUG
	std::cerr << "NEW GFXDEF: " << id << std::endl;
#endif
};

string gfxdef::get_id() const { return id; };

chrdef::chrdef(string id, u16 width, u16 height, u8 bitplanes,
							 vector<u32> planeoffset, vector<u32> pixeloffset,
							 vector<u32> rowoffset)
		: gfxdef(std::move(id)), datasize(width * height * bitplanes), width(width),
			height(height), bitplanes(bitplanes), planeoffset(std::move(planeoffset)),
			pixeloffset(std::move(pixeloffset)), rowoffset(std::move(rowoffset)){};

u16 chrdef::get_width() const { return width; }
u16 chrdef::get_height() const { return height; }
u8 chrdef::get_bitplanes() const { return bitplanes; }
u32 chrdef::get_pixeloffset_at(size_t pos) const { return pixeloffset[pos]; };
u32 chrdef::get_rowoffset_at(size_t pos) const { return rowoffset[pos]; };
u32 chrdef::get_planeoffset_at(size_t pos) const { return planeoffset[pos]; };
u32 chrdef::get_datasize() const { return datasize; }

coldef::coldef(string id, u8 bitdepth, vector<rgb_layout> layout,
							 bool is_big_endian)
		: gfxdef(std::move(id)), bitdepth(bitdepth), layout(std::move(layout)),
			is_big_endian(is_big_endian), is_reftab(false){};

coldef::coldef(string id, palette reftab, bool is_big_endian)
		: gfxdef(std::move(id)), reftab(std::move(reftab)),
			is_big_endian(is_big_endian), is_reftab(true){};

rgb_layout coldef::get_rgb_pass(size_t pass) const { return layout[pass]; }

u8 coldef::get_bitdepth() const { return bitdepth; };

vector<rgb_layout> coldef::get_rgb_layout() const { return layout; }

color coldef::get_reftab_entry(size_t index) const { return reftab[index]; }

size_t coldef::get_reftab_idx(color rgb) const
{
	size_t idx{0};
	for(auto &this_color : reftab) {
		if(this_color.red == rgb.red && this_color.green == rgb.green &&
			 this_color.blue == rgb.blue) {
			return idx;
		}
		++idx;
	}

	// this could certainly use some tuning, but it mostly works
	std::vector<std::pair<int, int>> distances;
	distances.reserve(this->reftab.size());
	int pal_color_iter{0};
	for(const auto &this_color : this->reftab) {
		int this_distance = (abs(this_color.red - rgb.red)) +
												(abs(this_color.green - rgb.green)) +
												(abs(this_color.blue - rgb.blue));
		distances.push_back(std::pair<int, int>(pal_color_iter, this_distance));
		++pal_color_iter;
	}

	int dist_check{distances[0].second};
	idx = 0;
	for(const auto &this_entry : distances) {
		if(std::get<1>(this_entry) < dist_check) {
			dist_check = this_entry.second;
			idx = this_entry.first;
		}
	}

	return idx;
};

bool coldef::use_reftab() const { return is_reftab; }
bool coldef::get_is_big_endian() const { return is_big_endian; }

paldef::paldef(string id, u8 entry_datasize, u16 subpal_length,
							 u16 subpal_count, u8 subpal_datasize)
		: gfxdef(std::move(id)), entry_datasize(entry_datasize),
			subpal_length(subpal_length), subpal_count(subpal_count),
			subpal_datasize(subpal_datasize){};

u8 paldef::get_entry_datasize() const { return entry_datasize; }

u16 paldef::get_subpal_length() const { return subpal_length; }

u16 paldef::get_subpal_count() const { return subpal_count; }

u8 paldef::get_subpal_datasize() const { return subpal_datasize; }

rgb_layout::rgb_layout(pair<s8, u8> red, pair<s8, u8> green, pair<s8, u8> blue)
		: red(std::move(red)), green(std::move(green)), blue(std::move(blue)){};

s8 rgb_layout::get_red_shift() const { return red.first; }
u8 rgb_layout::get_red_count() const { return red.second; }
s8 rgb_layout::get_green_shift() const { return green.first; }
u8 rgb_layout::get_green_count() const { return green.second; }
s8 rgb_layout::get_blue_shift() const { return blue.first; }
u8 rgb_layout::get_blue_count() const { return blue.second; }
} // namespace chrgfx
