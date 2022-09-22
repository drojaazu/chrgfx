#include "paldef.hpp"

using namespace std;

namespace chrgfx
{

paldef::paldef(string const & id,
	ushort const entry_datasize,
	ushort const pal_length,
	optional<ushort const> const pal_datasize,
	string const & description) :
		gfxdef(id, description),
		m_pal_length(pal_length),
		m_entry_datasize(entry_datasize),
		m_subpal_datasize(
			pal_datasize ? pal_datasize.value() : entry_datasize * pal_length) {};

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