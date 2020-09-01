#ifndef CHRGFX_EXEC_SHARED_H
#define CHRGFX_EXEC_SHARED_H

#include "chrgfx.hpp"
#include <getopt.h>
#include <iostream>
#include <optional>
#include <string>
#include <vector>

/*
	Hardcoding /etc isn't best practice, I'm sure, but I couldn't find a standard
	way of doing it otherwise. Anyone with a better implementation is certainly
	welcome to open a PR.
*/
std::string const DEFAULT_LIB_PATH{"/etc/chrgfx"};
std::string const DEFAULT_GFXDEF_FILE{DEFAULT_LIB_PATH + "/gfxdefs"};

struct runtime_config {
	string profile;
	string gfxdef;
	string chrdef;
	string coldef;
	string paldef;
	std::optional<unsigned int> subpalette;

	runtime_config() : gfxdef(DEFAULT_GFXDEF_FILE), subpalette(std::nullopt){};
};

class gfxprofile : public chrgfx::gfxdef
{
public:
	gfxprofile(string const &id, string const &chrdef_id, string const &coldef_id,
						 string const &paldef_id)
			: chrgfx::gfxdef(std::move(id)), chrdef_id(std::move(chrdef_id)),
				coldef_id(std::move(coldef_id)), paldef_id(std::move(paldef_id)){};

	string get_chrdef_id() const { return chrdef_id; }
	string get_coldef_id() const { return coldef_id; }
	string get_paldef_id() const { return paldef_id; }

private:
	string chrdef_id;
	string coldef_id;
	string paldef_id;
};

// these are intentionally mutable
extern string default_short_opts;

extern std::vector<option> default_long_opts;

string ltrim(const string &s);
string rtrim(const string &s);
string trim(const string &s);

bool process_default_args(runtime_config &cfg, int argc, char **argv);

#endif