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
 *
 */
class coldef : public gfxdef
{
public:
	coldef() = delete;

	bool big_endian() const;
	coldef_type type() const;

protected:
	coldef_type m_type;
	const bool m_big_endian;

	coldef(std::string const & id, coldef_type type, bool const big_endian);
};

/**
 * @brief Encoding for non-RGB based colors
 *
 */
class refcoldef : public coldef
{
public:
	refcoldef(std::string const & id, png::palette const & reftab,
						bool const big_endian = false);

	/**
	 * @brief
	 *
	 * @param index
	 * @return color Returns the color from the reference palette for the given
	 * index
	 */
	png::color reftabColor(ushort index) const;

	/**
	 * Returns the index to the color matching the RGB value provided, or its
	 * nearest color
	 */
	ushort reftabIndex(png::color rgb) const;

	png::palette const & reftab() const;

protected:
	png::palette const m_reftab;
};

class rgbcoldef : public coldef
{

public:
	/**
	 * Constructor for an rgblayout based coldef
	 */
	rgbcoldef(std::string const &, ushort const bitdepth,
						std::vector<rgb_layout> const & layout,
						bool const big_endian = false);

	/**
	 * Returns the vector of RGB layouts
	 */
	std::vector<rgb_layout> const & layout() const;

	/**
	 * Returns the RGB bit layout for the given pass
	 */
	rgb_layout const & rgb_pass(ushort pass) const;

	/**
	 * Returns the bitdepth of the color channels
	 */
	ushort bitdepth() const;

protected:
	std::vector<rgb_layout> const m_layout;
	ushort const m_bitdepth;
};
} // namespace chrgfx

#endif