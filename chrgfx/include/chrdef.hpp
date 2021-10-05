#ifndef CHRGFX__CHRDEF_H
#define CHRGFX__CHRDEF_H

#include "gfxdef.hpp"
#include <string>
#include <vector>

using namespace std;

namespace chrgfx
{

	/**
	 * \class chrdef
	 * \brief Defines the pixel layout of a graphics tile
	 */
	class chrdef : public gfxdef
	{

	public:
		chrdef(string id, ushort width, ushort height, ushort bitdepth,
					 vector<ushort> planeoffset, vector<ushort> pixeloffset,
					 vector<ushort> rowoffset) :
				gfxdef(id),
				width(width), height(height), bitdepth(bitdepth),
				datasize(width * height * bitdepth), planeoffset(planeoffset),
				pixeloffset(pixeloffset), rowoffset(rowoffset) {};

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
		 * \return bit offset to a given bitplane in a row
		 */
		ushort planeOffsetAt(ushort index) const;

		/**
		 * \return offset to a given pixel in a row, in bits
		 */
		ushort pixelOffsetAt(ushort index) const;

		/**
		 * \return offset to a given row in the tile, in bits
		 */
		ushort rowOffsetAt(ushort index) const;

		/**
		 * \return data size of a single tile, *in bits*
		 */
		ushort datasize() const;
	};

} // namespace chrgfx

#endif