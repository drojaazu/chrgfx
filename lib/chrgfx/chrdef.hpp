/**
 * @file chrdef.hpp
 * @author Damian Rogers / damian@motoi.pro
 * @copyright ©2022 Motoi Productions / Released under MIT License
 * @brief Tile format definition
 */

#ifndef __CHRGFX__CHRDEF_HPP
#define __CHRGFX__CHRDEF_HPP

#include "gfxdef.hpp"
#include "types.hpp"
#include <string>
#include <vector>

namespace chrgfx
{

/**
 * @brief Tile encoding
 */
class chrdef : public gfxdef
{
protected:
	uint m_width;
	uint m_height;
	uint m_bitdepth;
	uint m_datasize;
	uint m_datasize_bytes;
	std::vector<uint> m_pixeloffsets;
	std::vector<uint> m_rowoffsets;
	std::vector<uint> m_planeoffsets;

public:
	chrdef(std::string const & id,
		uint const width,
		uint const height,
		uint const bitdepth,
		std::vector<uint> const & pixel_offsets,
		std::vector<uint> const & row_offsets,
		std::vector<uint> const & plane_offsets,
		std::string const & description = "");

	/**
	 * @return uint Width of the tile in pixels
	 */
	[[nodiscard]] uint width() const;

	/**
	 * @return uint Height of the tile in pixels
	 */
	[[nodiscard]] uint height() const;

	/**
	 * @return uint Bit depth of the tile
	 */
	[[nodiscard]] uint bpp() const;

	/**
	 * @return uint Data size of a single tile *in bits*
	 */
	[[nodiscard]] uint datasize() const;

	/**
	 * @return uint Data size of a single tile *in bytes*
	 */
	[[nodiscard]] uint datasize_bytes() const;

	/**
	 * @return Reference to the collection of bit offsets to bitplanes within a
	 * row
	 */
	[[nodiscard]] std::vector<uint> const & plane_offsets() const;

	/**
	 * @return Reference to the collection of bit offsets to pixels within a
	 * row
	 */
	[[nodiscard]] std::vector<uint> const & pixel_offsets() const;

	/**
	 * @return Reference to the collection of bit offsets to rows within a tile
	 */
	[[nodiscard]] std::vector<uint> const & row_offsets() const;

	/**
	 * @return uint Bit offset to specified bitplane index within a row
	 */
	[[nodiscard]] uint plane_offset_at(uint bitplane_index) const;

	/**
	 * @return uint Bit offset to specified pixel index within a row
	 */
	[[nodiscard]] uint pixel_offset_at(uint pixel_index) const;

	/**
	 * @return uint Bit offset to specified row index within a tile
	 */
	[[nodiscard]] uint row_offset_at(uint row_index) const;
};

} // namespace chrgfx

#endif
