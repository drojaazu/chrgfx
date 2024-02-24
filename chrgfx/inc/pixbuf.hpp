/**
 * @file pixmap.hpp
 * @author your name (you@domain.com)
 * @brief
 * @version 0.1
 * @date 2024-02-21
 *
 * @copyright Copyright (c) 2024
 *
 */

#ifndef __CHRGFX__PIXBUF_HPP
#define __CHRGFX__PIXBUF_HPP

#include "blob.hpp"
#include "types.hpp"
#include <array>
#include <cstddef>
#include <cstdint>
#include <stdexcept>
#include <vector>

namespace chrgfx
{
using pixel_buffer = motoi::blob<uint8_t>;

/**
 * @brief pixel buffer, intermediate format to be converted into something more useful
 * such as png, bmp or chunked into tile graphics
 * pixel format is 8bpp packed
 * put simply, each byte is an unsigned value that represents an index into the color palette
 *
 */
class image
{
private:
	uint m_width;
	uint m_height;
	uint m_datasize;
	// we represent the data as ints instead of byte as they *are* typed, being the index into the color palette
	pixel_buffer m_pixbuf;

public:
	image(uint const width, uint const height, uint8_t const * pixel_buffer) :
			m_width {width},
			m_height {height},
			m_datasize {width * height},
			m_pixbuf {pixel_buffer, m_datasize}
	{
		if (m_datasize == 0)
			throw std::runtime_error("invalid width and/or height specified for pixmap");
	}

	image(uint const width, uint const height) :
			m_width {width},
			m_height {height},
			m_datasize {width * height},
			m_pixbuf(m_datasize, 0)
	{
		if (m_datasize == 0)
			throw std::runtime_error("invalid width and/or height specified for pixmap");
	}

	[[nodiscard]] auto width() const
	{
		return m_width;
	}

	[[nodiscard]] auto height() const
	{
		return m_height;
	}

	auto pixbuf()
	{
		return m_pixbuf.data();
	}

	[[nodiscard]] auto const pixbuf() const
	{
		return m_pixbuf.data();
	}
};

class color
{
public:
	uint8 red {0};
	uint8 green {0};
	uint8 blue {0};

	color() = default;

	explicit color(uint8 red, uint8 green, uint8 blue) :
			red {red},
			green {green},
			blue {blue}
	{
	}

	color(color const & other)
	{
		this->red = other.red;
		this->green = other.green;
		this->blue = other.blue;
	}

	std::string to_html()
	{
		// TODO
		return "";
	}
};

using palette = std::array<color, 256>;

} // namespace chrgfx

#endif