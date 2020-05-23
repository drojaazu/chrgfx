#ifndef CHRGFX_EXEC_SHARED_H
#define CHRGFX_EXEC_SHARED_H

#include "chrgfx.hpp"
#include <getopt.h>
#include <iostream>
#include <string>
#include <vector>

using std::string;

struct runtime_config {
	string profile{""};
	string gfxdef{""};
	s16 subpalette;

	runtime_config() { subpalette = -1; }
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

std::pair<string, string> kvsplit(const string &line, const char delim = ' ');

bool process_default_args(runtime_config &cfg, int argc, char **argv);

template <typename T> T sto(string const &str);

#endif