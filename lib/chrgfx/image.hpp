/**
 * @file image.hpp
 * @author Motoi Productions (Damian Rogers damian@motoi.pro)
 * @copyright 2024 Motoi Productions / MIT License
 * @brief Classes and utilities related to "standard" graphics, i.e. the intermediary format used internally for
 * conversion between other encoded types and commonly used image formats
 *
 */

#ifndef __MOTOI__IMAGE_HPP
#define __MOTOI__IMAGE_HPP

#include "strutil.hpp"
#include "types.hpp"
#include <array>
#include <stdexcept>

namespace motoi
{
/**
 * @brief Each pixel in the "standard" format is an index into an 8 bit color table
 * (this makes the standard format 8bpp packed)
 *
 */

namespace pixel_type
{
struct rgb
{
public:
	uint8_t red {0};
	uint8_t green {0};
	uint8_t blue {0};

	rgb() = default;
	explicit rgb(uint8_t red, uint8_t green, uint8_t blue) :
			red(red),
			green(green),
			blue(blue)
	{
	}

	rgb(std::string_view color)
	{
		if (color[0] == '#')
			color = color.begin() + 1;
		if (color.length() != 6)
			throw std::runtime_error("invalid hex triplet specified for color (shorthand format not supported)");

		red = sto<uint8_t>(color.substr(0, 2), 16);
		green = sto<uint8_t>(color.substr(2, 2), 16);
		blue = sto<uint8_t>(color.substr(4, 2), 16);
	}

	std::string to_html()
	{
		std::ostringstream oss;
		oss << '#' << std::hex << std::setfill('0') << std::setw(2) << (unsigned int) red << std::setw(2)
				<< (unsigned int) green << std::setw(2) << (unsigned int) blue;
		return oss.str();
	}
};

using index = uint8_t;

} // namespace pixel_type

template <uint length>
using basic_color_map = std::array<pixel_type::rgb, length>;

/**
 * @brief A standard 8 bit color table
 *
 */
using color_map_8bpp = basic_color_map<256>;

/**
 * @brief Represents a self-contained viewable image (pixel data and color table) in a "standard" intermediate format
 *
 * @warning does not do bounds checking on the pixel_buffer for the given width/height
 *
 */
template <class pixel_type, class color_map_t = color_map_8bpp>
class image
{
private:
	uint m_width;
	uint m_height;
	size_t m_datasize;
	pixel_type * m_pixmap {nullptr};
	color_map_t * m_colormap {nullptr};

public:
	image(uint const width, uint const height) :
			m_width {width},
			m_height {height},
			m_datasize {width * height}
	{
		if (m_datasize == 0)
			throw std::runtime_error("invalid width and/or height specified for pixmap");

		m_pixmap = new pixel_type[m_datasize];
	}

	[[nodiscard]] uint width() const
	{
		return m_width;
	}

	[[nodiscard]] uint height() const
	{
		return m_height;
	}

	pixel_type const * pixel_map_row(size_t row_index) const
	{
		return m_pixmap + (m_width * row_index);
	}

	pixel_type * pixel_map_row(size_t row_index)
	{
		return m_pixmap + (m_width * row_index);
	}

	pixel_type * pixel_map()
	{
		return m_pixmap;
	}

	pixel_type const * pixel_map() const
	{
		return m_pixmap;
	}

	[[nodiscard]] color_map_t const * color_map() const
	{
		return m_colormap;
	}

	color_map_t * color_map()
	{
		return m_colormap;
	}

	void color_map(color_map_t const & color_map)
	{
		m_colormap = new color_map_t(color_map);
	}

	~image()
	{
		delete[] m_pixmap;
		delete m_colormap;
	}
};

} // namespace motoi
#endif