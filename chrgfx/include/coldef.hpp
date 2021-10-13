#ifndef CHRGFX__COLDEF_H
#define CHRGFX__COLDEF_H

#include <png++/png.hpp>
#include <string>
#include <vector>

#include "gfxdef.hpp"
#include "rgb_layout.hpp"

namespace chrgfx
{
using namespace std;
using namespace png;

/**
 * Specifying a table of RGB values that approximately correspond to the
 * output of the hardware (reftab based)
 */
class refcoldef : public gfxdef
{
protected:
	palette m_reftab;
	const bool m_big_endian;

public:
	refcoldef(string id, palette reftab, bool is_big_endian = false) :
			gfxdef(std::move(id)), m_reftab(std::move(reftab)),
			m_big_endian(is_big_endian) {};

	/**
	 * Returns the color from the reference palette for the given index
	 */
	color reftabColor(ushort index) const;

	/**
	 * Returns the index to the color matching the RGB value provided, or its
	 * nearest color
	 */
	ushort reftabIndex(color rgb) const;

	/**
	 * \brief Returns true if the original harware is big endian (reftab based)
	 */
	bool big_endian() const;
};

/**
 * Defines color data in one of two ways:
 *   - Specifying offsets and width of each RGB color channel within the
 * data (rgblayout based)

 */
class rgbcoldef : public gfxdef
{
protected:
	vector<rgb_layout> m_layout;
	ushort m_bitdepth;
	const bool m_big_endian;

public:
	/**
	 * Constructor for an rgblayout based coldef
	 */
	rgbcoldef(string id, ushort bitdepth, vector<rgb_layout> layout,
						bool is_big_endian = false) :
			gfxdef(std::move(id)),
			m_bitdepth(bitdepth), m_layout(std::move(layout)),
			m_big_endian(is_big_endian) {};

	/**
	 * Returns the vector of RGB layouts (rgblayout based)
	 */
	vector<rgb_layout> const & rgb_layout() const;

	/**
	 * Returns the RGB bit layout for the given pass (rgblayout based)
	 */
	rgb_layout get_rgb_pass(ushort pass) const;

	/**
	 * Returns the bitdepth of the color channels
	 */
	ushort bitdepth() const;

	/**
	 * Returns true if the original harware is big endian (reftab based)
	 */
	bool big_endian() const;
};
} // namespace chrgfx

#endif