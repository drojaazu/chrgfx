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
#include <string>

namespace chrgfx
{
/**
 * @brief Each pixel in the "standard" format is an index into an 8 bit color table
 * (this makes the standard format 8bpp packed)
 *
 */
using basic_pixel = uint8_t;

/**
 * @brief Represents an RGB color
 *
 */
class basic_color
{
public:
	uint8_t red {0};
	uint8_t green {0};
	uint8_t blue {0};

	basic_color() = default;
	basic_color(basic_color const & other) = default;
	basic_color(std::string_view color);
	basic_color(std::string const & color);

	explicit basic_color(uint8_t red, uint8_t green, uint8_t blue);

	std::string to_html();
};

/**
 * @brief A standard 8 bit color table
 *
 */
using basic_palette = std::array<basic_color, 256>;

/**
 * @brief Represents a self-contained viewable image (pixel data and color table) in a "standard" intermediate format
 *
 * @warning does not do bounds checking on the pixel_buffer for the given width/height
 *
 */
class basic_image
{
private:
	uint m_width;
	uint m_height;
	uint m_datasize;
	basic_pixel * m_pixbuf {nullptr};
	basic_palette * m_palette {nullptr};

public:
	basic_image(uint const width, uint const height);

	[[nodiscard]] uint width() const;

	[[nodiscard]] uint height() const;

	[[nodiscard]] basic_pixel const * pixbuf() const;

	basic_pixel * pixbuf();

	[[nodiscard]] chrgfx::basic_palette const * palette() const;

	chrgfx::basic_palette * palette();

	void palette(basic_palette const & pal);

	~basic_image();
};

} // namespace chrgfx
#endif