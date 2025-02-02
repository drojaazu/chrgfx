/**
 * @file rgb_layout.hpp
 * @author Damian Rogers / damian@motoi.pro
 * @copyright ©2022 Motoi Productions / Released under MIT License
 * @brief RGB color levels data layout
 */

#ifndef __CHRGFX__RGB_LAYOUT_HPP
#define __CHRGFX__RGB_LAYOUT_HPP

#include "types.hpp"
#include <utility>

using std::pair;

namespace chrgfx
{
/**
 * @brief Defines the bit positions of RGB channel data
 * @note Positive shift values shift right; negative values shift left
 */
class rgb_layout
{
public:
	rgb_layout(pair<short, uint> const & red, pair<short, uint> const & green, pair<short, uint> const & blue);

	[[nodiscard]] short red_offset() const;

	[[nodiscard]] uint red_size() const;

	[[nodiscard]] short green_offset() const;

	[[nodiscard]] uint green_size() const;

	[[nodiscard]] short blue_offset() const;

	[[nodiscard]] uint blue_size() const;

protected:
	pair<short, uint> m_red;
	pair<short, uint> m_green;
	pair<short, uint> m_blue;
};

} // namespace chrgfx

#endif