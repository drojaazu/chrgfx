#ifndef CHRGFX__CHRDEF_HPP
#define CHRGFX__CHRDEF_HPP

#include "gfxdef.hpp"
#include <string>
#include <vector>

namespace chrgfx
{

/**
 * @brief Tile encoding
 */
class chrdef : public gfxdef
{

public:
	chrdef(std::string const & id,
		ushort width,
		ushort height,
		ushort bitdepth,
		std::vector<ushort> const & planeoffset,
		std::vector<ushort> const & pixeloffset,
		std::vector<ushort> const & rowoffset,
		std::string const & description = "");

	/**
	 * @return ushort Width of the tile in pixels
	 */
	[[nodiscard]] ushort width() const;

	/**
	 * @return ushort Height of the tile in pixels
	 */
	[[nodiscard]] ushort height() const;

	/**
	 * @return ushort Bit depth of the tile
	 */
	[[nodiscard]] ushort bitdepth() const;

	/**
	 * @return ushort Data size of a single tile *in bits*
	 */
	[[nodiscard]] ushort datasize() const;

	/**
	 * @return Reference to the collection of bit offsets to bitplanes within a
	 * row
	 */
	[[nodiscard]] std::vector<ushort> const & plane_offsets() const;

	/**
	 * @return Reference to the collection of bit offsets to pixels within a
	 * row
	 */
	[[nodiscard]] std::vector<ushort> const & pixel_offsets() const;

	/**
	 * @return Reference to the collection of bit offsets to rows within a tile
	 */
	[[nodiscard]] std::vector<ushort> const & row_offsets() const;

	/**
	 * @return ushort Bit offset to specified bitplane index within a row
	 */
	[[nodiscard]] ushort plane_offset_at(ushort bitplane_index) const;

	/**
	 * @return ushort Bit offset to specified pixel index within a row
	 */
	[[nodiscard]] ushort pixel_offset_at(ushort pixel_index) const;

	/**
	 * @return ushort Bit offset to specified row index within a tile
	 */
	[[nodiscard]] ushort row_offset_at(ushort row_index) const;

protected:
	ushort const m_width;
	ushort const m_height;
	ushort const m_bitdepth;
	ushort const m_datasize;

	std::vector<ushort> m_planeoffsets;
	std::vector<ushort> m_pixeloffsets;
	std::vector<ushort> m_rowoffsets;
};

} // namespace chrgfx

#endif
