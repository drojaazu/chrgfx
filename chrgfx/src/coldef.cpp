#include "coldef.hpp"

namespace chrgfx
{
using namespace std;
using namespace png;

color refcoldef::reftabColor(ushort index) const
{
	return m_reftab[index];
};

ushort refcoldef::reftabIndex(color rgb) const
{
	size_t idx { 0 };
	for(auto & this_color : m_reftab)
	{
		if(this_color.red == rgb.red && this_color.green == rgb.green &&
			 this_color.blue == rgb.blue)
		{
			return idx;
		}
		++idx;
	}

	// this could certainly use some tuning, but it mostly works
	vector<pair<int, int>> distances;
	distances.reserve(this->m_reftab.size());
	int pal_color_iter { 0 };
	for(const auto & this_color : this->m_reftab)
	{
		int this_distance = (abs(this_color.red - rgb.red)) +
												(abs(this_color.green - rgb.green)) +
												(abs(this_color.blue - rgb.blue));
		distances.push_back(pair<int, int>(pal_color_iter, this_distance));
		++pal_color_iter;
	}

	int dist_check { distances[0].second };
	idx = 0;
	for(const auto & this_entry : distances)
	{
		if(get<1>(this_entry) < dist_check)
		{
			dist_check = this_entry.second;
			idx = this_entry.first;
		}
	}

	return idx;
};

bool refcoldef::big_endian() const
{
	return m_big_endian;
};

vector<rgb_layout> const & rgbcoldef::layout() const
{
	return m_layout;
};

rgb_layout rgbcoldef::rgb_pass(ushort pass) const
{
	return m_layout[pass];
}

ushort rgbcoldef::bitdepth() const
{
	return m_bitdepth;
};

bool rgbcoldef::big_endian() const
{
	return m_big_endian;
};

} // namespace chrgfx
