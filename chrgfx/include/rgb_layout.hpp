#ifndef CHRGFX__RGB_LAYOUT_H
#define CHRGFX__RGB_LAYOUT_H

#include "types.hpp"
#include <utility>

using std::pair;

namespace chrgfx
{
	/**
	 * \class rgb_layout
	 * \brief Defines the bit positions of RGB channel data
	 * Positive shift values shift right; negative values shift left
	 */
	class rgb_layout
	{
	public:
		rgb_layout(pair<int, unsigned int> red, pair<int, unsigned int> green,
							 pair<int, unsigned int> blue);

		int get_red_shift() const
		{
			return red.first;
		}

		unsigned int get_red_size() const
		{
			return red.second;
		}

		int get_green_shift() const
		{
			return green.first;
		}

		unsigned int get_green_size() const
		{
			return green.second;
		}

		int get_blue_shift() const
		{
			return blue.first;
		}

		unsigned int get_blue_size() const
		{
			return blue.second;
		}

	protected:
		pair<int, unsigned int> red;
		pair<int, unsigned int> green;
		pair<int, unsigned int> blue;
	};

} // namespace chrgfx

#endif