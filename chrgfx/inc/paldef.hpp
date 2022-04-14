#ifndef CHRGFX__PALDEF_HPP
#define CHRGFX__PALDEF_HPP

#include "gfxdef.hpp"
#include <optional>
#include <string>

namespace chrgfx
{

/**
 * @brief Palette encoding
 */
class paldef : public gfxdef
{
public:
	paldef(std::string const & id, ushort const entry_datasize,
				 ushort const pal_length,
				 std::optional<ushort const> const pal_datasize = std::nullopt);

	/**
	 * @return number of entries in the palette
	 */
	ushort pal_length() const;

	/**
	 * @return data size of a single entry *in bits*
	 */
	ushort entry_datasize() const;

	/**
	 * @return data size of the whole palette *in bits*
	 */
	ushort datasize() const;

protected:
	ushort const m_pal_length;
	ushort const m_entry_datasize;
	ushort const m_subpal_datasize;
};

} // namespace chrgfx

#endif
