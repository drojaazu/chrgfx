/**
 * @file basic_gfx.hpp
 * @author Motoi Productions (Damian Rogers damian@motoi.pro)
 * @copyright 2024 Motoi Productions / MIT License
 * @brief Classes and utilities related to "standard" graphics, i.e. the intermediary format used internally for
 * conversion between other encoded types and commonly used image formats
 *
 */

#ifndef __CHRGFX__STDGFX_HPP
#define __CHRGFX__STDGFX_HPP

#include "types.hpp"
#include <array>
#include <stdexcept>

namespace chrgfx
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
};

using index_pixel = uint8_t;

} // namespace pixel_type

/**
 * @brief A standard 8 bit color table
 *
 */
using palette = std::array<pixel_type::rgb, 256>;

/**
 * @brief Represents a self-contained viewable image (pixel data and color table) in a "standard" intermediate format
 *
 * @warning does not do bounds checking on the pixel_buffer for the given width/height
 *
 */
template <class pixel_type>
class image
{
private:
	uint m_width;
	uint m_height;
	size_t m_datasize;
	pixel_type * m_pixmap {nullptr};
	palette * m_palette {nullptr};

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

	[[nodiscard]] pixel_type const * pixmap() const
	{
		return m_pixmap;
	}

	pixel_type * pixmap()
	{
		return m_pixmap;
	}

	[[nodiscard]] chrgfx::palette const * palette() const
	{
		return m_palette;
	}

	chrgfx::palette * palette()
	{
		return m_palette;
	}

	void palette(::chrgfx::palette const & pal)
	{
		m_palette = new ::chrgfx::palette(pal);
	}

	~image()
	{
		delete[] m_pixmap;
		delete m_palette;
	}
};

} // namespace chrgfx
#endif