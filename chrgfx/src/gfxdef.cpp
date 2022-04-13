#include "gfxdef.hpp"
#ifdef DEBUG
#include <iostream>
#endif

using namespace std;

namespace chrgfx
{
gfxdef::gfxdef(string const & id) : m_id(id)
{
#ifdef DEBUG
	cerr << "NEW GFXDEF: " << m_id << endl;
#endif
};

string const & gfxdef::id() const
{
	return m_id;
};

} // namespace chrgfx