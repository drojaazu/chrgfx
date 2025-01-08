#include "basic_gfx.hpp"
#include "strutil.hpp"
#include <stdexcept>

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

basic_color::basic_color(string_view color)
{
	if (color[0] == '#')
		color = color.begin() + 1;
	if (color.length() != 6)
		throw runtime_error("invalid hex triplet specified for color (shorthand format not supported)");

	red = sto<uint8_t>(color.substr(0, 2), 16);
	green = sto<uint8_t>(color.substr(2, 2), 16);
	blue = sto<uint8_t>(color.substr(4, 2), 16);
}

basic_color::basic_color(string const & color) :
		basic_color::basic_color(string_view(color))
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
		m_datasize {width * height}
{
	if (m_datasize == 0)
		throw runtime_error("invalid width and/or height specified for pixmap");

	m_pixbuf = new basic_pixel[m_datasize];
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
	return m_pixbuf;
}

basic_pixel * basic_image::pixbuf()
{
	return m_pixbuf;
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

basic_image::~basic_image()
{
	delete[] m_pixbuf;
}

} // namespace chrgfx
