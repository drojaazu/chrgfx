#ifndef CHRGFX__SHARED_GFXPROFILE_HPP
#define CHRGFX__SHARED_GFXPROFILE_HPP

#include "gfxdef.hpp"
#include <string>

using chrgfx::gfxdef;
using std::string;

class gfxprofile : public gfxdef
{
public:
	gfxprofile (string const & id, string chrdef_id, string coldef_id, string paldef_id, string const & description = "");

	[[nodiscard]] string chrdef_id () const;

	[[nodiscard]] string coldef_id () const;

	[[nodiscard]] string paldef_id () const;

private:
	string m_chrdef_id;
	string m_coldef_id;
	string m_paldef_id;
};

#endif
