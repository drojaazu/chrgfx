#ifndef CHRGFX__CHRDEF_HPP
#define CHRGFX__CHRDEF_HPP

#include "gfxdef.hpp"
#include <string>
#include <vector>

using std::vector;

namespace chrgfx
{

/**
 * @brief Tile encoding
 *
 */
class chrdef : public gfxdef
{

public:
	chrdef(char const * id, ushort const width, ushort const height,
				 ushort const bitdepth, vector<ushort> const & planeoffset,
				 vector<ushort> const & pixeloffset, vector<ushort> const & rowoffset);

	/**
	 * @return ushort Width of the tile in pixels
	 */
	ushort width() const;

	/**
	 * @return ushort Height of the tile in pixels
	 */
	ushort height() const;

	/**
	 * @return ushort Bit depth of the tile
	 */
	ushort bitdepth() const;

	/**
	 * @return ushort Data size of a single tile *in bits*
	 */
	ushort datasize() const;

	/**
	 * @return ushort Bit offset to specified bitplane in a row
	 */
	ushort plane_offset(ushort const bitplane_index) const;

	/**
	 * @return ushort Bit offset to specified pixel in a row
	 */
	ushort pixel_offset(ushort const pixel_index) const;

	/**
	 * @return ushort Bit offset to specified row in the tile
	 */
	ushort row_offset(ushort const row_index) const;

protected:
	ushort const m_width;
	ushort const m_height;
	ushort const m_bitdepth;

	vector<ushort> m_planeoffset;

	vector<ushort> m_pixeloffset;

	vector<ushort> m_rowoffset;

	ushort const m_datasize;
};

} // namespace chrgfx

#endif