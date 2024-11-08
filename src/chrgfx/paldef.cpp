#include "paldef.hpp"

using namespace std;

namespace chrgfx
{

paldef::paldef(string const & id,
	uint const entry_datasize,
	uint const pal_length,
	optional<uint const> const pal_datasize,
	string const & description) :
		gfxdef(id, description),
		m_pal_length(pal_length),
		m_entry_datasize(entry_datasize),
		m_subpal_datasize(pal_datasize ? pal_datasize.value() : entry_datasize * pal_length) {};

uint paldef::length() const
{
	return m_pal_length;
}

uint paldef::entry_datasize() const
{
	return m_entry_datasize;
}

uint paldef::datasize() const
{
	return m_subpal_datasize;
}

} // namespace chrgfx