#ifndef CHRGFX__PALDEF_HPP
#define CHRGFX__PALDEF_HPP

#include "gfxdef.hpp"
#include <optional>
#include <string>

using std::nullopt;
using std::optional;

namespace chrgfx
{

/**
 * \class paldef
 * \brief Defines the structure of a system palette
 */
class paldef : public gfxdef
{
public:
	paldef(char const * id, ushort const entry_datasize, ushort const pal_length,
				 optional<ushort const> const pal_datasize = nullopt);

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

protected:
	// ushort m_pal_length;
	ushort const m_pal_length;
	ushort const m_entry_datasize;
	ushort const m_subpal_datasize;
};

} // namespace chrgfx

#endif
