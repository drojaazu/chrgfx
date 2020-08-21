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

chrdef::chrdef(string const &id, u16 const width, u16 const height,
							 u8 const bitplanes, vector<u32> const &planeoffset,
							 vector<u32> const &pixeloffset, vector<u32> const &rowoffset)
		: gfxdef(std::move(id)), datasize(width * height * bitplanes),
			width(std::move(width)), height(std::move(height)),
			bitplanes(std::move(bitplanes)), planeoffset(std::move(planeoffset)),
			pixeloffset(std::move(pixeloffset)), rowoffset(std::move(rowoffset))

{
}

u16 chrdef::get_width() const { return width; }
u16 chrdef::get_height() const { return height; }
u8 chrdef::get_bitplanes() const { return bitplanes; }
u32 chrdef::get_pixeloffset_at(size_t pos) const { return pixeloffset[pos]; };
u32 chrdef::get_rowoffset_at(size_t pos) const { return rowoffset[pos]; };
u32 chrdef::get_planeoffset_at(size_t pos) const { return planeoffset[pos]; };
u32 chrdef::get_datasize() const { return datasize; }

coldef::coldef(const string id, const vector<rgb_layout> layout,
							 bool is_big_endian)
		: gfxdef(std::move(id)), layout(std::move(layout)),
			is_big_endian(std::move(is_big_endian)), is_refpal(false){};

coldef::coldef(const string id, const palette &refpal, bool is_big_endian)
		: gfxdef(std::move(id)), refpal(std::move(refpal)),
			is_big_endian(std::move(is_big_endian)), is_refpal(true){};

rgb_layout coldef::get_rgb_pass(size_t pass) const { return layout[pass]; }

vector<rgb_layout> coldef::get_rgb_layout() const { return layout; }

color coldef::get_refpal_entry(size_t index) const { return refpal[index]; }

size_t coldef::get_refpal_idx(color rgb) const
{
	size_t idx{0};
	for(auto this_color : refpal) {
		if(this_color.red == rgb.red && this_color.green == rgb.green &&
			 this_color.blue == rgb.blue) {
			return idx;
		}
		++idx;
	}

	// this could certainly use some tuning, but it mostly works
	std::vector<std::tuple<int, int>> distances;
	distances.reserve(this->refpal.size());
	int pal_color_iter{0};
	for(const auto &this_color : this->refpal) {
		int this_distance = (abs(this_color.red - rgb.red)) +
												(abs(this_color.green - rgb.green)) +
												(abs(this_color.blue - rgb.blue));
		distances.push_back(std::tuple<int, int>(pal_color_iter, this_distance));
		++pal_color_iter;
	}

	int dist_check{std::get<1>(distances[0])};
	idx = 0;
	for(const auto &this_entry : distances) {
		if(std::get<1>(this_entry) < dist_check) {
			dist_check = std::get<1>(this_entry);
			idx = std::get<0>(this_entry);
		}
	}

	return idx;
};

bool coldef::use_refpal() const { return is_refpal; }
bool coldef::get_is_big_endian() const { return is_big_endian; }

paldef::paldef(string const &id, u8 const entry_datasize,
							 u16 const subpal_length, u16 const subpal_count,
							 u8 const subpal_datasize)
		: gfxdef(std::move(id)), entry_datasize(std::move(entry_datasize)),
			subpal_length(std::move(subpal_length)),
			subpal_count(std::move(subpal_count)),
			subpal_datasize(std::move(subpal_datasize)){};

u8 paldef::get_entry_datasize() const { return entry_datasize; }

u16 paldef::get_subpal_length() const { return subpal_length; }

u16 paldef::get_subpal_count() const { return subpal_count; }

u8 paldef::get_subpal_datasize() const { return subpal_datasize; }

rgb_layout::rgb_layout(pair<s8, u8> red, pair<s8, u8> green, pair<s8, u8> blue)
		: red(red), green(green), blue(blue){};

s8 rgb_layout::get_red_shift() const { return red.first; }
u8 rgb_layout::get_red_count() const { return red.second; }
s8 rgb_layout::get_green_shift() const { return green.first; }
u8 rgb_layout::get_green_count() const { return green.second; }
s8 rgb_layout::get_blue_shift() const { return blue.first; }
u8 rgb_layout::get_blue_count() const { return blue.second; }
} // namespace chrgfx