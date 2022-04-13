#include "paldef.hpp"

namespace chrgfx
{

paldef::paldef(std::string const & id, ushort const entry_datasize,
							 ushort const pal_length,
							 std::optional<ushort const> const pal_datasize) :
		gfxdef(id),
		m_entry_datasize(entry_datasize), m_pal_length(pal_length),
		m_subpal_datasize(pal_datasize ? pal_datasize.value()
																	 : entry_datasize * pal_length) {};

ushort paldef::pal_length() const
{
	return m_pal_length;
}

ushort paldef::entry_datasize() const
{
	return m_entry_datasize;
}

ushort paldef::datasize() const
{
	return m_subpal_datasize;
}

} // namespace chrgfx