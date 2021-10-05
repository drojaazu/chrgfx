#include "paldef.hpp"

namespace chrgfx
{
	ushort paldef::getPalLength() const
	{
		return palLength;
	};

	ushort paldef::getSubpalLength() const
	{
		return subpalLength;
	};

	ushort paldef::getSubpalCount() const
	{
		return subpalCount;
	};

	ushort paldef::getEntryDatasize() const
	{
		return entryDatasize;
	};

	ushort paldef::getPaletteDatasize() const
	{
		return paletteDatasize;
	};

	ushort paldef::getSubpalDatasize() const
	{
		return subpalDatasize;
	}

} // namespace chrgfx