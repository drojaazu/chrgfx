#ifndef CHRGFX__PALDEF_H
#define CHRGFX__PALDEF_H

#include "gfxdef.hpp"
#include <optional>
#include <string>

namespace chrgfx
{
	using namespace std;

	/**
	 * \class paldef
	 * \brief Defines the structure of a system palette
	 */
	class paldef : public gfxdef
	{

	protected:
		ushort palLength;
		ushort subpalLength;
		ushort subpalCount;

		ushort entryDatasize;
		ushort subpalDatasize;
		ushort paletteDatasize;

	public:
		paldef(string id, ushort const entryDatasize, ushort const subpalLength,
					 ushort const subpalCount,
					 optional<ushort> const subpal_datasize = nullopt) :
				gfxdef(id),
				entryDatasize(entryDatasize), subpalLength(subpalLength),
				subpalCount(subpalCount), palLength(subpalLength * subpalCount),
				subpalDatasize(subpal_datasize ? subpal_datasize.value()
																			 : entryDatasize * subpalLength),
				paletteDatasize(subpalDatasize * subpalCount) {};

		/**
		 * \return number of entries in the system palette
		 */
		ushort getPalLength() const;

		/**
		 * \return number of entries in a subpalette
		 */
		ushort getSubpalLength() const;

		/**
		 * \return number of subpalettes in the system palette
		 */
		ushort getSubpalCount() const;

		/**
		 * \return data size of a single entry, *in bits*
		 */
		ushort getEntryDatasize() const;

		/**
		 * \return data size of the system palette, *in bits*
		 */
		ushort getPaletteDatasize() const;

		/**
		 * \return data size of a subpalette, *in bits*
		 */
		ushort getSubpalDatasize() const;
	};

} // namespace chrgfx

#endif
