#include "gfxprofile.hpp"

gfxprofile::gfxprofile(char const * id, string const & chrdef_id,
											 string const & coldef_id, string const & paldef_id) :
		gfxdef(id),
		m_chrdef_id(chrdef_id), m_coldef_id(coldef_id), m_paldef_id(paldef_id) {};

string gfxprofile::chrdef_id() const
{
	return m_chrdef_id;
}

string gfxprofile::coldef_id() const
{
	return m_coldef_id;
}

string gfxprofile::paldef_id() const
{
	return m_paldef_id;
}
