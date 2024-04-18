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

#include "blob.hpp"
#include "chrdef.hpp"
#include "types.hpp"
#include <array>
#include <cstdint>
#include <optional>

namespace chrgfx
{
/**
 * @brief Each pixel in the "standard" format is an index into an 8 bit color table
 * (this makes the standard format 8bpp packed)
 *
 */
using basic_pixel = uint8_t;

/**
 * @brief Represents 
 *
 */
using basic_pixel_buffer = motoi::blob<basic_pixel>;

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

	explicit basic_color(uint8_t red, uint8_t green, uint8_t blue);

	std::string to_html();
};

/**
 * @brief A standard 8 bit color table
 *
 */
using basic_palette = std::array<basic_color, 256>;

/**
 * @brief Represents a fully viewable image (pixel data and color table) in a "standard" intermediate format
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
	basic_pixel_buffer m_pixbuf;
	basic_palette m_palette;

public:
	basic_image(uint const width, uint const height);

	[[nodiscard]] uint width() const;

	[[nodiscard]] uint height() const;

	[[nodiscard]] basic_pixel const * pixbuf() const;

	basic_pixel * pixbuf();

	[[nodiscard]] chrgfx::basic_palette const palette() const;

	chrgfx::basic_palette palette();

	void palette(basic_palette const & pal);
};

uint const DEFAULT_ROW_SIZE = 16;
bool const DEFAULT_DRAW_BORDER = false;
bool const DEFAULT_USE_TRNS = false;

/**
 * @brief Tile rendering settings
 */
struct render_config
{
public:
	/**
	 * @brief Number of tiles per row in the output image
	 *
	 */
	uint row_size {DEFAULT_ROW_SIZE};

	/**
	 * @brief Draw a 1 pixel border around the inner edges of tiles in the
	 * transparent entry color
	 */
	bool draw_border {DEFAULT_DRAW_BORDER};

	/**
	 * @brief Palette entry to use for transparency
	 * Also used as the border color when rendering
	 */
	std::optional<uint8> trns_index {std::nullopt};
};

/**
 * @brief Renders a collection of tiles to a basic image
 */
basic_image render_tileset(
	chrgfx::chrdef const & chrdef, motoi::blob<byte_t> const & chrdata, render_config const & rcfg);

/**
 * @brief Returns a collection of basic tiles segmented from a given image
 */
motoi::blob<byte_t> segment_tileset(chrgfx::chrdef const & chrdef, basic_image const & bitmap);

} // namespace chrgfx
#endif