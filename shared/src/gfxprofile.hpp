#ifndef CHRGFX__SHARED_GFXPROFILE_HPP
#define CHRGFX__SHARED_GFXPROFILE_HPP

#include "gfxdef.hpp"
#include <string>

using chrgfx::gfxdef;
using std::string;

class gfxprofile : public gfxdef
{
public:
	gfxprofile(string const & id, string const & chrdef_id,
						 string const & coldef_id, string const & paldef_id);

	string chrdef_id() const;

	string coldef_id() const;

	string paldef_id() const;

private:
	string m_chrdef_id;
	string m_coldef_id;
	string m_paldef_id;
};

#endif
