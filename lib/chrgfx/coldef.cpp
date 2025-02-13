#include "coldef.hpp"
#include <string>
#include <vector>

namespace chrgfx
{
using namespace std;

coldef::coldef(string const & id, coldef_type const type, bool const big_endian, string const & description) :
		gfxdef(id, description),
		m_type(type),
		m_big_endian(big_endian)
{
}

refcoldef::refcoldef(string const & id, palette refpal, bool const big_endian, string const & description) :
		coldef(id, ref, big_endian, description),
		m_refpal(refpal)
{
}

rgb_color refcoldef::by_value(uint const index) const
{
	return m_refpal[index];
};

uint refcoldef::by_color(rgb_color const & rgb) const
{
	// search for an exact match first
	size_t idx {0};
	for (auto const & this_color : m_refpal)
	{
		if (this_color.red == rgb.red && this_color.green == rgb.green && this_color.blue == rgb.blue)
		{
			return idx;
		}
		++idx;
	}

	// this could certainly use some tuning, but it mostly works
	vector<pair<int, int>> distances;
	distances.reserve(this->m_refpal.size());
	int pal_color_iter {0};
	for (const auto & this_color : this->m_refpal)
	{
		int this_distance =
			(abs(this_color.red - rgb.red)) + (abs(this_color.green - rgb.green)) + (abs(this_color.blue - rgb.blue));
		distances.emplace_back(pal_color_iter, this_distance);
		++pal_color_iter;
	}

	int dist_check {distances[0].second};
	idx = 0;
	for (const auto & this_entry : distances)
	{
		if (get<1>(this_entry) < dist_check)
		{
			dist_check = this_entry.second;
			idx = this_entry.first;
		}
	}

	return idx;
};

palette const & refcoldef::refpal() const
{
	return m_refpal;
}

bool coldef::big_endian() const
{
	return m_big_endian;
};

coldef_type coldef::type() const
{
	return m_type;
}

rgbcoldef::rgbcoldef(string const & id,
	uint const bitdepth,
	vector<rgb_layout> const & layout,
	bool const big_endian,
	string const & description) :
		coldef(id, rgb, big_endian, description),
		m_layout(layout),
		m_bitdepth(bitdepth) {};

vector<rgb_layout> const & rgbcoldef::layout() const
{
	return m_layout;
};

rgb_layout const & rgbcoldef::rgb_pass(uint pass) const
{
	return m_layout[pass];
}

uint rgbcoldef::bitdepth() const
{
	return m_bitdepth;
};

} // namespace chrgfx
