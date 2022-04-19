#include "gfxdef.hpp"
#ifdef DEBUG
#include <iostream>
#endif

using namespace std;

namespace chrgfx
{
gfxdef::gfxdef(string const & id, string const & description) :
		m_id(id), m_desc(description)
{
#ifdef DEBUG
	cerr << "NEW GFXDEF: " << m_id << endl;
#endif
};

string const & gfxdef::id() const
{
	return m_id;
};

string const & gfxdef::description() const
{
	return m_desc;
}

} // namespace chrgfx