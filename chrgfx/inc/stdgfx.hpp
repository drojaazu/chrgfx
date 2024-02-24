/**
 * @file stdgfx.hpp
 * @author Motoi Productions (Damian Rogers damian@motoi.pro)
 * @copyright 2024 Motoi Productions / MIT License
 * @brief Classes and utilities related to "standard" graphics, i.e. the intermediary format used internally for
 * conversion between other encoded types and commonly used image formats
 *
 */

#ifndef __CHRGFX__STDGFX_HPP
#define __CHRGFX__STDGFX_HPP

#include "blob.hpp"
#include <array>
#include <cstdint>

namespace chrgfx
{
/**
 * @brief Each pixel in the "standard" format is an index into an 8 bit color table
 * (this makes the standard format 8bpp packed)
 *
 */
using pixel_t = uint8_t;

/**
 * @brief A chunk of pixel data
 *
 */
using pixel_buffer = motoi::blob<pixel_t>;

/**
 * @brief An RGB color
 *
 */
class color
{
public:
	uint8_t red {0};
	uint8_t green {0};
	uint8_t blue {0};

	color() = default;
	color(color const & other) = default;

	explicit color(uint8_t red, uint8_t green, uint8_t blue);

	std::string to_html();
};

/**
 * @brief A standard 8 bit color table
 *
 */
using palette = std::array<color, 256>;

/**
 * @brief Represents a fully viewable image (pixel data and color table) in the "standard" intermediate format
 *
 * @warning does not do bounds checking on the pixel_buffer for the given width/height
 *
 */
class image
{
private:
	uint m_width;
	uint m_height;
	uint m_datasize;
	pixel_buffer m_pixbuf;
	palette m_palette;

public:
	image(uint const width, uint const height, pixel_t const * pixel_buffer);

	image(uint const width, uint const height);

	[[nodiscard]] uint width() const;

	[[nodiscard]] uint height() const;

	[[nodiscard]] pixel_t const * pixbuf() const;

	pixel_t * pixbuf();

	[[nodiscard]] chrgfx::palette const palette() const;

	chrgfx::palette palette();
};
} // namespace chrgfx
#endif