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
		ushort width;
		ushort height;
		ushort bitdepth;

		vector<ushort> planeoffset;
		//ushort const * planeoffset_data;

		vector<ushort> pixeloffset;
		//ushort const * pixeloffset_data;

		vector<ushort> rowoffset;
		//ushort const * rowoffset_data;

		ushort datasize; // size of one chr in bits

	public:
		chrdef(string id, ushort width, ushort height,
					 ushort bitdepth, vector<ushort> planeoffset,
					 vector<ushort> pixeloffset, vector<ushort> rowoffset) :
				gfxdef(id),
				width(width), height(height), bitdepth(bitdepth),
				datasize(width * height * bitdepth), planeoffset(planeoffset),
				pixeloffset(pixeloffset), rowoffset(rowoffset) {};

		/**
		 * \return width of the tile, in pixels
		 */
		ushort getWidth() const
		{
			return width;
		}

		/**
		 * \return height of the tile, in pixels
		 */
		ushort getHeight() const
		{
			return height;
		}

		/**
		 * \return bit depth of the tile
		 */
		ushort getBitdepth() const
		{
			return bitdepth;
		}

		/**
		 * \return bit offset to a given bitplane in a row
		 */
		ushort getPlaneOffsetAt(ushort index) const
		{
			return planeoffset[index];
		};

		/**
		 * \return offset to a given pixel in a row, in bits
		 */
		ushort getPixelOffsetAt(ushort index) const
		{
			return pixeloffset[index];
		};

		/**
		 * \return offset to a given row in the tile, in bits
		 */
		ushort getRowOffsetAt(ushort index) const
		{
			return rowoffset[index];
		};

		/**
		 * \return data size of a single tile, *in bits*
		 */
		ushort getDataSize() const
		{
			return datasize;
		}
	};

} // namespace chrgfx

#endif