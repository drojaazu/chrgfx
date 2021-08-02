#ifndef CHRGFX_EXEC_SHARED_H
#define CHRGFX_EXEC_SHARED_H

#include "chrgfx.hpp"
#include <getopt.h>
#include <iostream>
#include <optional>
#include <string>
#include <vector>

using namespace std;

static string const APP_CONTACT { "Damian R (damian@sudden-desu.net)" };
static string const APP_WEBSITE { "https://github.com/drojaazu/chrgfx" };

/*
	Hardcoding /etc isn't best practice, I'm sure, but I couldn't find a standard
	way of doing it otherwise. Anyone with a better implementation is certainly
	welcome to open a PR.
*/
string const DEFAULT_LIB_PATH { "/etc/chrgfx" };
string const DEFAULT_GFXDEF_FILE { DEFAULT_LIB_PATH + "/gfxdefs" };

struct runtime_config
{
	string profile;
	string gfxdefs_file;
	string chrdef;
	string coldef;
	string paldef;
	optional<unsigned int> subpalette;

	runtime_config() : gfxdefs_file(DEFAULT_GFXDEF_FILE), subpalette(nullopt) {};
};

class gfxprofile : public chrgfx::gfxdef
{
public:
	gfxprofile(string const & id, string const & chrdef_id,
						 string const & coldef_id, string const & paldef_id) :
			gfxdef(id),
			chrdef_id(chrdef_id), coldef_id(coldef_id), paldef_id(paldef_id) {};

	string get_chrdef_id() const
	{
		return chrdef_id;
	}
	string get_coldef_id() const
	{
		return coldef_id;
	}
	string get_paldef_id() const
	{
		return paldef_id;
	}

private:
	string chrdef_id;
	string coldef_id;
	string paldef_id;
};

// these are intentionally mutable
extern string default_short_opts;

extern vector<option> default_long_opts;

string ltrim(const string & s);
string rtrim(const string & s);
string trim(const string & s);

bool process_default_args(runtime_config & cfg, int argc, char ** argv);

#endif