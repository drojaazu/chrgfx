#include "gfxdef.hpp"
#ifdef DEBUG
#include <iostream>
#endif

using namespace std;

namespace chrgfx
{
gfxdef::gfxdef (string id, string description) :
		m_id (std::move (id)),
		m_desc (std::move (description))
{
#ifdef DEBUG
	cerr << "NEW GFXDEF: " << m_id << endl;
#endif
};

string const & gfxdef::id () const
{
	return m_id;
};

string const & gfxdef::description () const
{
	return m_desc;
}

} // namespace chrgfx