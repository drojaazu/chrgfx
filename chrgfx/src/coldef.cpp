#include "coldef.hpp"

namespace chrgfx
{
using namespace std;
using namespace png;

color refcoldef::reftabColor(ushort index) const
{
	return p_reftab[index];
};

ushort refcoldef::reftabIndex(color rgb) const
{
	size_t idx { 0 };
	for(auto & this_color : p_reftab)
	{
		if(this_color.red == rgb.red && this_color.green == rgb.green &&
			 this_color.blue == rgb.blue)
		{
			return idx;
		}
		++idx;
	}

	// this could certainly use some tuning, but it mostly works
	std::vector<std::pair<int, int>> distances;
	distances.reserve(this->p_reftab.size());
	int pal_color_iter { 0 };
	for(const auto & this_color : this->p_reftab)
	{
		int this_distance = (abs(this_color.red - rgb.red)) +
												(abs(this_color.green - rgb.green)) +
												(abs(this_color.blue - rgb.blue));
		distances.push_back(std::pair<int, int>(pal_color_iter, this_distance));
		++pal_color_iter;
	}

	int dist_check { distances[0].second };
	idx = 0;
	for(const auto & this_entry : distances)
	{
		if(std::get<1>(this_entry) < dist_check)
		{
			dist_check = this_entry.second;
			idx = this_entry.first;
		}
	}

	return idx;
};

bool refcoldef::bigEndian() const
{
	return p_big_endian;
};

vector<rgb_layout> const & rgbcoldef::rgbLayout() const
{
	return layout;
};

rgb_layout rgbcoldef::get_rgb_pass(ushort pass) const
{
	return layout[pass];
}

ushort rgbcoldef::getBitdepth() const
{
	return bitdepth;
};

bool rgbcoldef::get_is_big_endian() const
{
	return is_big_endian;
};

} // namespace chrgfx
