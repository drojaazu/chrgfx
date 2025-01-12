#include "paldef.hpp"

using namespace std;

namespace chrgfx
{

paldef::paldef(string const & id,
	uint const entry_datasize,
	uint const length,
	optional<uint const> const datasize,
	string const & description) :
		gfxdef(id, description),
		m_length(length),
		m_entry_datasize(entry_datasize),
		m_entry_datasize_bytes(m_entry_datasize / 8 + (m_entry_datasize % 8 > 0 ? 1 : 0)),
		m_datasize(datasize ? datasize.value() : entry_datasize * length),
		m_datasize_bytes(m_datasize / 8 + (m_datasize % 8 > 0 ? 1 : 0)) {};

paldef::paldef(string const & id, uint const entry_datasize, uint const length, string const & description) :
		paldef(id, entry_datasize, length, std::nullopt, description) {};

uint paldef::length() const
{
	return m_length;
}

uint paldef::entry_datasize() const
{
	return m_entry_datasize;
}

uint paldef::entry_datasize_bytes() const
{
	return m_entry_datasize_bytes;
}

uint paldef::datasize() const
{
	return m_datasize;
}

uint paldef::datasize_bytes() const
{
	return m_datasize_bytes;
}

} // namespace chrgfx