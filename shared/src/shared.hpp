#ifndef CHRGFX__SHARED_SHARED_HPP
#define CHRGFX__SHARED_SHARED_HPP

#include "chrgfx.hpp"
#include <getopt.h>
#include <iostream>
#include <optional>
#include <string>
#include <vector>

using namespace std;

/*
	Hardcoding /etc isn't best practice, I'm sure, but I couldn't find a standard
	way of doing it otherwise. Anyone with a better implementation is certainly
	welcome to open a PR.
*/
extern string const CONFIG_PATH;
extern string const GFXDEF_PATH;

// these are intentionally mutable
extern string default_short_opts;

extern vector<option> default_long_opts;

struct runtime_config
{
	string profile;
	string gfxdefs_path;
	string chrdef_id;
	string coldef_id;
	string paldef_id;
	optional<unsigned int> subpalette;
};

bool process_default_args(runtime_config const & cfg, int argc, char ** argv);

#endif
