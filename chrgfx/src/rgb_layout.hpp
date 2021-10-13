#ifndef CHRGFX__RGB_LAYOUT_H
#define CHRGFX__RGB_LAYOUT_H

#include "types.hpp"
#include <utility>

using std::pair;

namespace chrgfx
{
/**
 * \brief Defines the bit positions of RGB channel data
 * Positive shift values shift right; negative values shift left
 */
class rgb_layout
{
public:
	rgb_layout(pair<short, ushort> red, pair<short, ushort> green,
						 pair<short, ushort> blue);

	short red_shift() const
	{
		return m_red.first;
	}

	ushort red_size() const
	{
		return m_red.second;
	}

	short green_shift() const
	{
		return m_green.first;
	}

	ushort green_size() const
	{
		return m_green.second;
	}

	short blue_shift() const
	{
		return m_blue.first;
	}

	ushort blue_size() const
	{
		return m_blue.second;
	}

protected:
	pair<short, ushort> m_red;
	pair<short, ushort> m_green;
	pair<short, ushort> m_blue;
};

} // namespace chrgfx

#endif