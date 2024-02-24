#include "stdgfx.hpp"

namespace chrgfx
{

using namespace std;

color::color(uint8_t red, uint8_t green, uint8_t blue) :
		red {red},
		green {green},
		blue {blue}
{
}

string color::to_html()
{
	// TODO
	return "";
}

image::image(uint const width, uint const height, pixel_t const * pixel_buffer) :
		m_width {width},
		m_height {height},
		m_datasize {width * height},
		m_pixbuf {pixel_buffer, m_datasize}
{
	if (m_datasize == 0)
		throw runtime_error("invalid width and/or height specified for pixmap");
}

image::image(uint const width, uint const height) :
		m_width {width},
		m_height {height},
		m_datasize {width * height},
		m_pixbuf(m_datasize, 0)
{
	if (m_datasize == 0)
		throw runtime_error("invalid width and/or height specified for pixmap");
}

[[nodiscard]] uint image::width() const
{
	return m_width;
}

[[nodiscard]] uint image::height() const
{
	return m_height;
}

[[nodiscard]] pixel_t const * image::pixbuf() const
{
	return m_pixbuf.data();
}

pixel_t * image::pixbuf()
{
	return m_pixbuf.data();
}

[[nodiscard]] palette const image::palette() const
{
	return m_palette;
}

palette image::palette()
{
	return m_palette;
}

} // namespace chrgfx
