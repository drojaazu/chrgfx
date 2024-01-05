#include "gfxprofile.hpp"

gfxprofile::gfxprofile(
	string const & id, string chrdef_id, string coldef_id, string paldef_id, string const & description) :
		gfxdef(id, description),
		m_chrdef_id(std::move(chrdef_id)),
		m_coldef_id(std::move(coldef_id)),
		m_paldef_id(std::move(paldef_id))
{
}

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
