#include "gfxdef.hpp"
#ifdef DEBUG
#include <iostream>
#endif

using namespace std;

namespace chrgfx
{
gfxdef::gfxdef(char const * id) : m_id(id)
{
#ifdef DEBUG
	cerr << "NEW GFXDEF: " << m_id << endl;
#endif
};

char const * gfxdef::id() const
{
	return m_id;
};

} // namespace chrgfx