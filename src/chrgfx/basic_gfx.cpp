#include "basic_gfx.hpp"

#include <utility>
#ifdef DEBUG
#include <iostream>
#endif

namespace chrgfx
{

using namespace std;
using namespace motoi;

basic_color::basic_color(uint8_t red, uint8_t green, uint8_t blue) :
		red {red},
		green {green},
		blue {blue}
{
}

string basic_color::to_html()
{
	// TODO
	return "";
}

basic_image::basic_image(uint const width, uint const height) :
		m_width {width},
		m_height {height},
		m_datasize {width * height},
		m_pixbuf {m_datasize, 0}
{
	if (m_datasize == 0)
		throw runtime_error("invalid width and/or height specified for pixmap");
}

[[nodiscard]] uint basic_image::width() const
{
	return m_width;
}

[[nodiscard]] uint basic_image::height() const
{
	return m_height;
}

[[nodiscard]] basic_pixel const * basic_image::pixbuf() const
{
	return m_pixbuf.data();
}

basic_pixel * basic_image::pixbuf()
{
	return m_pixbuf.data();
}

[[nodiscard]] basic_palette const basic_image::palette() const
{
	return m_palette;
}

basic_palette basic_image::palette()
{
	return m_palette;
}

void basic_image::palette(basic_palette const & pal)
{
	m_palette = pal;
}


} // namespace chrgfx
