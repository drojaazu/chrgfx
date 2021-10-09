#include "paldef.hpp"

namespace chrgfx
{

ushort paldef::subpal_size() const
{
	return subpalLength;
}

ushort paldef::entry_datasize() const
{
	return entryDatasize;
}

ushort paldef::datasize() const
{
	return subpalDatasize;
}

ushort paldef::datasize_bytes() const
{
	return subpalDatasize >> 3;
}

} // namespace chrgfx