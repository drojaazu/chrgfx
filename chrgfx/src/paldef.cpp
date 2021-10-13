#include "paldef.hpp"

namespace chrgfx
{

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