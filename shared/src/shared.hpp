#ifndef CHRGFX__SHARED_SHARED_HPP
#define CHRGFX__SHARED_SHARED_HPP

#include "chrdef.hpp"
#include "coldef.hpp"
#include "import_defs.hpp"
#include "paldef.hpp"
#include "usage.hpp"
#include <getopt.h>
#include <iostream>
#include <string>
#include <vector>

/*
	Hardcoding /etc isn't best practice, I'm sure, but I couldn't find a standard
	way of doing it otherwise. Anyone with a better implementation is certainly
	welcome to open a PR.
*/
extern std::string const CONFIG_PATH;
extern std::string const GFXDEF_PATH;

// these are intentionally mutable
extern std::string short_opts;
extern std::vector<option> long_opts;
extern std::vector<option_details> opt_details;

struct runtime_config
{
	std::string profile;
	std::string gfxdefs_path;
	std::string chrdef_id;
	std::string coldef_id;
	std::string paldef_id;
	bool list_gfxdefs;

	runtime_config ();
};

class def_helper
{
public:
	chrgfx::chrdef const * chrdef;
	chrgfx::coldef const * coldef;
	chrgfx::paldef const * paldef;

	def_helper (runtime_config & cfg);

	void list_gfxdefs (std::ostream & os);

private:
	def_collection m_defs;
};

bool shared_args (char this_opt, runtime_config & cfg);

#endif
