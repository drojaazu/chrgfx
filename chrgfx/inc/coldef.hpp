#ifndef CHRGFX__COLDEF_HPP
#define CHRGFX__COLDEF_HPP

#include "gfxdef.hpp"
#include "rgb_layout.hpp"
#include <png++/png.hpp>
#include <vector>

namespace chrgfx
{
using png::color;
using png::palette;
using std::vector;

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

	explicit coldef(char const * id, coldef_type type, bool const big_endian);
};

/**
 * @brief Encoding for non-RGB based colors
 *
 */
class refcoldef : public coldef
{
public:
	refcoldef(char const * id, palette const & reftab,
						bool const big_endian = false);

	/**
	 * @brief
	 *
	 * @param index
	 * @return color Returns the color from the reference palette for the given
	 * index
	 */
	color reftabColor(ushort index) const;

	/**
	 * Returns the index to the color matching the RGB value provided, or its
	 * nearest color
	 */
	ushort reftabIndex(color rgb) const;

protected:
	palette const m_reftab;
};

class rgbcoldef : public coldef
{

public:
	/**
	 * Constructor for an rgblayout based coldef
	 */
	rgbcoldef(char const * id, ushort const bitdepth,
						vector<rgb_layout> const & layout, bool const big_endian = false);

	/**
	 * Returns the vector of RGB layouts
	 */
	vector<rgb_layout> const & layout() const;

	/**
	 * Returns the RGB bit layout for the given pass
	 */
	rgb_layout rgb_pass(ushort pass) const;

	/**
	 * Returns the bitdepth of the color channels
	 */
	ushort bitdepth() const;

protected:
	vector<rgb_layout> const m_layout;
	ushort const m_bitdepth;
};
} // namespace chrgfx

#endif