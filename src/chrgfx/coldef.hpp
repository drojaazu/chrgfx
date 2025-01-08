/**
 * @file coldef.hpp
 * @author Damian Rogers / damian@motoi.pro
 * @copyright ©2022 Motoi Productions / Released under MIT License
 * @brief Color format definition
 */

#ifndef __CHRGFX__COLDEF_HPP
#define __CHRGFX__COLDEF_HPP

#include "basic_gfx.hpp"
#include "gfxdef.hpp"
#include "rgb_layout.hpp"
#include <vector>

namespace chrgfx
{

enum coldef_type
{
	ref,
	rgb
};

/**
 * @brief Abstract class for color encodings
 */
class coldef : public gfxdef
{
public:
	coldef() = delete;

	[[nodiscard]] bool big_endian() const;
	[[nodiscard]] coldef_type type() const;

protected:
	coldef_type m_type;
	bool m_big_endian;

	coldef(std::string const & id, coldef_type type, bool big_endian, std::string const & description = "");
};

/**
 * @brief Non-RGB (reference palette based) color encoding
 */
class refcoldef : public coldef
{
public:
	refcoldef(
		std::string const & id, basic_palette refpal, bool big_endian = false, std::string const & description = "");

	/**
	 * @return color color from the reference palette for the given index
	 */
	[[nodiscard]] basic_color by_value(uint const index) const;

	/**
	 * @return index to the color matching the RGB value provided, or
	 * the index to the nearest matching color
	 */
	[[nodiscard]] uint by_color(basic_color const & rgb) const;

	[[nodiscard]] basic_palette const & refpal() const;

protected:
	basic_palette const m_refpal;
};

/**
 * @brief RGB color encoding
 *
 */
class rgbcoldef : public coldef
{
public:
	rgbcoldef(std::string const &,
		uint bitdepth,
		std::vector<rgb_layout> const & layout,
		bool big_endian = false,
		std::string const & description = "");

	/**
	 * @return referece to the collection of RGB layouts
	 */
	[[nodiscard]] std::vector<rgb_layout> const & layout() const;

	/**
	 * @return RGB bit layout for the given pass index
	 */
	[[nodiscard]] rgb_layout const & rgb_pass(uint pass) const;

	/**
	 * @return bitdepth of the color channels
	 */
	[[nodiscard]] uint bitdepth() const;

protected:
	std::vector<rgb_layout> const m_layout;
	uint const m_bitdepth;
};
} // namespace chrgfx

#endif
