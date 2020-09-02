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

chrdef::chrdef(string id, uint width, uint height, uint bitplanes,
							 vector<uint> planeoffset, vector<uint> pixeloffset,
							 vector<uint> rowoffset, conv_chr::cvto_chr_t converter_to,
							 conv_chr::cvfrom_chr_t converter_from)
		: gfxdef(std::move(id)), datasize(width * height * bitplanes), width(width),
			height(height), bitplanes(bitplanes), planeoffset(std::move(planeoffset)),
			pixeloffset(std::move(pixeloffset)), rowoffset(std::move(rowoffset)),
			converter_to(converter_to), converter_from(converter_from){};

uint chrdef::get_width() const { return width; }
uint chrdef::get_height() const { return height; }
uint chrdef::get_bitplanes() const { return bitplanes; }
uint chrdef::get_pixeloffset_at(uint pos) const { return pixeloffset[pos]; };
uint chrdef::get_rowoffset_at(uint pos) const { return rowoffset[pos]; };
uint chrdef::get_planeoffset_at(uint pos) const { return planeoffset[pos]; };
uint chrdef::get_datasize() const { return datasize; }
conv_chr::cvfrom_chr_t chrdef::get_converter_from() const
{
	return converter_from;
};
conv_chr::cvto_chr_t chrdef::get_converter_to() const { return converter_to; };

coldef::coldef(string id, uint bitdepth, vector<rgb_layout> layout,
							 bool is_big_endian, conv_color::cvto_col_t converter_to,
							 conv_color::cvfrom_col_t converter_from)
		: gfxdef(std::move(id)), bitdepth(bitdepth), layout(std::move(layout)),
			is_big_endian(is_big_endian), is_reftab(false),
			converter_to(converter_to), converter_from(converter_from){};

coldef::coldef(string id, palette reftab, bool is_big_endian,
							 conv_color::cvto_col_t converter_to,
							 conv_color::cvfrom_col_t converter_from)
		: gfxdef(std::move(id)), reftab(std::move(reftab)),
			is_big_endian(is_big_endian), is_reftab(true), converter_to(converter_to),
			converter_from(converter_from){};

rgb_layout coldef::get_rgb_pass(uint pass) const { return layout[pass]; }
uint coldef::get_bitdepth() const { return bitdepth; };
vector<rgb_layout> coldef::get_rgb_layout() const { return layout; }
color coldef::get_reftab_entry(uint index) const { return reftab[index]; }
uint coldef::get_reftab_idx(color rgb) const
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

conv_color::cvfrom_col_t coldef::get_converter_from() const
{
	return converter_from;
};

conv_color::cvto_col_t coldef::get_converter_to() const
{
	return converter_to;
};

paldef::paldef(string id, uint entry_datasize, uint subpal_length,
							 uint subpal_count, std::optional<uint> subpal_datasize,
							 conv_palette::cvto_pal_t converter_to,
							 conv_palette::cvfrom_pal_t converter_from)
		: gfxdef(std::move(id)), entry_datasize(entry_datasize),
			subpal_length(subpal_length), subpal_count(subpal_count),
			subpal_datasize(subpal_datasize ? subpal_datasize.value()
																			: entry_datasize * subpal_length),
			converter_to(converter_to), converter_from(converter_from){};

uint paldef::get_entry_datasize() const { return entry_datasize; }
uint paldef::get_palette_length() const
{
	return subpal_length * subpal_count;
};
uint paldef::get_subpal_length() const { return subpal_length; }
uint paldef::get_subpal_count() const { return subpal_count; }
uint paldef::get_palette_datasize() const
{
	return subpal_datasize * subpal_count;
}
uint paldef::get_palette_datasize_bytes() const
{
	return (subpal_datasize * subpal_count) / 8;
}
uint paldef::get_subpal_datasize() const { return subpal_datasize; }
uint paldef::get_subpal_datasize_bytes() const { return subpal_datasize / 8; };

conv_palette::cvto_pal_t paldef::get_converter_to() const
{
	return converter_to;
};

conv_palette::cvfrom_pal_t paldef::get_converter_from() const
{
	return converter_from;
};

rgb_layout::rgb_layout(pair<int, uint> red, pair<int, uint> green,
											 pair<int, uint> blue)
		: red(std::move(red)), green(std::move(green)), blue(std::move(blue)){};

int rgb_layout::get_red_shift() const { return red.first; }
uint rgb_layout::get_red_count() const { return red.second; }
int rgb_layout::get_green_shift() const { return green.first; }
uint rgb_layout::get_green_count() const { return green.second; }
int rgb_layout::get_blue_shift() const { return blue.first; }
uint rgb_layout::get_blue_count() const { return blue.second; }
} // namespace chrgfx
