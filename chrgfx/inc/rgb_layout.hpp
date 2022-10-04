#ifndef CHRGFX__RGB_LAYOUT_HPP
#define CHRGFX__RGB_LAYOUT_HPP

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
	rgb_layout (pair<short, ushort> const & red, pair<short, ushort> const & green, pair<short, ushort> const & blue);

	[[nodiscard]] short red_shift () const;

	[[nodiscard]] ushort red_size () const;

	[[nodiscard]] short green_shift () const;

	[[nodiscard]] ushort green_size () const;

	[[nodiscard]] short blue_shift () const;

	[[nodiscard]] ushort blue_size () const;

protected:
	pair<short, ushort> const m_red;
	pair<short, ushort> const m_green;
	pair<short, ushort> const m_blue;
};

} // namespace chrgfx

#endif