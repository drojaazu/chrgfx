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
	// ushort m_pal_length;
	ushort m_pal_length;
	ushort m_entry_datasize;
	ushort m_subpal_datasize;

public:
	paldef(string const & id, ushort const entry_datasize,
				 ushort const pal_length,
				 optional<ushort> const pal_datasize = nullopt) :
			gfxdef(id),
			m_entry_datasize(entry_datasize), m_pal_length(pal_length),
			m_subpal_datasize(pal_datasize ? pal_datasize.value()
																		 : entry_datasize * pal_length) {};

	/**
	 * \return number of entries in a subpalette
	 */
	ushort pal_length() const;

	/**
	 * \return data size of a single entry, *in bits*
	 */
	ushort entry_datasize() const;

	/**
	 * \return data size of the subpalette, *in bits*
	 */
	ushort datasize() const;
};

} // namespace chrgfx

#endif
