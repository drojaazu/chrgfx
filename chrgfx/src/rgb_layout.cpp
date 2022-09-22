#include "rgb_layout.hpp"

using namespace std;

namespace chrgfx
{

rgb_layout::rgb_layout(pair<short, ushort> const & red,
	pair<short, ushort> const & green,
	pair<short, ushort> const & blue) :
		m_red(red),
		m_green(green),
		m_blue(blue)
{
}

short rgb_layout::red_shift() const
{
	return m_red.first;
}

ushort rgb_layout::red_size() const
{
	return m_red.second;
}

short rgb_layout::green_shift() const
{
	return m_green.first;
}

ushort rgb_layout::green_size() const
{
	return m_green.second;
}

short rgb_layout::blue_shift() const
{
	return m_blue.first;
}

ushort rgb_layout::blue_size() const
{
	return m_blue.second;
}

} // namespace chrgfx
