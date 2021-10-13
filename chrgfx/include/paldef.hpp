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
	ushort p_palLength;
	ushort p_subpalLength;
	ushort p_entryDatasize;
	ushort p_subpalDatasize;

public:
	paldef(string id, ushort const entryDatasize, ushort const subpalLength,
				 optional<ushort> const subpal_datasize = nullopt) :
			gfxdef(id),
			p_entryDatasize(entryDatasize), p_subpalLength(subpalLength),
			p_subpalDatasize(subpal_datasize ? subpal_datasize.value()
																			 : entryDatasize * subpalLength) {};

	/**
	 * \return number of entries in a subpalette
	 */
	ushort subpalLength() const;

	/**
	 * \return data size of a single entry, *in bits*
	 */
	ushort entryDatasize() const;

	/**
	 * \return data size of the subpalette, *in bits*
	 */
	ushort datasize() const;
};

} // namespace chrgfx

#endif
