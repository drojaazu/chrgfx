#ifndef CHRGFX__COLDEF_HPP
#define CHRGFX__COLDEF_HPP

#include "gfxdef.hpp"
#include "rgb_layout.hpp"
#include <png++/png.hpp>
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

	bool big_endian() const;
	coldef_type type() const;

protected:
	coldef_type const m_type;
	bool const m_big_endian;

	coldef(std::string const & id, coldef_type const type, bool const big_endian,
				 std::string const & description = "");
};

/**
 * @brief Non-RGB (reftab based) color encoding
 */
class refcoldef : public coldef
{
public:
	refcoldef(std::string const & id, png::palette const & reftab,
						bool const big_endian = false,
						std::string const & description = "");

	/**
	 * @return color color from the reference palette for the given index
	 */
	png::color by_value(ushort index) const;

	/**
	 * @return index to the color matching the RGB value provided, or
	 * the index to the nearest matching color
	 */
	ushort by_color(png::color rgb) const;

	png::palette const & reftab() const;

protected:
	png::palette const m_reftab;
};

/**
 * @brief RGB color encoding
 *
 */
class rgbcoldef : public coldef
{
public:
	rgbcoldef(std::string const &, ushort const bitdepth,
						std::vector<rgb_layout> const & layout,
						bool const big_endian = false,
						std::string const & description = "");

	/**
	 * @return referece to the collection of RGB layouts
	 */
	std::vector<rgb_layout> const & layout() const;

	/**
	 * @return RGB bit layout for the given pass index
	 */
	rgb_layout const & rgb_pass(ushort pass) const;

	/**
	 * @return bitdepth of the color channels
	 */
	ushort bitdepth() const;

protected:
	std::vector<rgb_layout> const m_layout;
	ushort const m_bitdepth;
};
} // namespace chrgfx

#endif