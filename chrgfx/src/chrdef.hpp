#ifndef CHRGFX__CHRDEF_H
#define CHRGFX__CHRDEF_H

#include "gfxdef.hpp"
#include <string>
#include <vector>

using std::string;
using std::vector;

namespace chrgfx
{

	/**
	 * \class chrdef
	 * \brief Defines the pixel layout of a graphics tile
	 */
	class chrdef : public gfxdef
	{

	protected:
		ushort p_width;
		ushort p_height;
		ushort p_bitdepth;

		vector<ushort> p_planeoffset;
		// ushort const * planeoffset_data;

		vector<ushort> p_pixeloffset;
		// ushort const * pixeloffset_data;

		vector<ushort> p_rowoffset;
		// ushort const * rowoffset_data;

		ushort p_datasize; // size of one chr in bits

	public:
		chrdef(string id, ushort width, ushort height, ushort bitdepth,
					 vector<ushort> planeoffset, vector<ushort> pixeloffset,
					 vector<ushort> rowoffset) :
				gfxdef(id),
				p_width(width), p_height(height), p_bitdepth(bitdepth),
				p_datasize(width * height * bitdepth), p_planeoffset(planeoffset),
				p_pixeloffset(pixeloffset), p_rowoffset(rowoffset) {};

		/**
		 * \return width of the tile, in pixels
		 */
		ushort width() const;

		/**
		 * \return height of the tile, in pixels
		 */
		ushort height() const;

		/**
		 * \return bit depth of the tile
		 */
		ushort bitdepth() const;

		/**
		 * \return data size of a single tile, *in bits*
		 */
		ushort datasize() const;

		/**
		 * \return bit offset to a given bitplane in a row
		 */
		ushort planeOffsetAt(ushort index) const;

		/**
		 * \return bit offset to a given pixel in a row
		 */
		ushort pixelOffsetAt(ushort index) const;

		/**
		 * \return bit offset to a given row in the tile
		 */
		ushort rowOffsetAt(ushort index) const;
	};

} // namespace chrgfx

#endif