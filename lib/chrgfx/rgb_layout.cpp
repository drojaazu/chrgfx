#include "rgb_layout.hpp"

using namespace std;

namespace chrgfx
{

rgb_layout::rgb_layout(pair<short, uint> const & red, pair<short, uint> const & green, pair<short, uint> const & blue) :
		m_red(red),
		m_green(green),
		m_blue(blue)
{
}

short rgb_layout::red_shift() const
{
	return m_red.first;
}

uint rgb_layout::red_size() const
{
	return m_red.second;
}

short rgb_layout::green_shift() const
{
	return m_green.first;
}

uint rgb_layout::green_size() const
{
	return m_green.second;
}

short rgb_layout::blue_shift() const
{
	return m_blue.first;
}

uint rgb_layout::blue_size() const
{
	return m_blue.second;
}

} // namespace chrgfx
