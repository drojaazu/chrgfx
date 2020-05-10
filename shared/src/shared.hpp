#ifndef CHRGFX_EXEC_SHARED_H
#define CHRGFX_EXEC_SHARED_H

#include "chrgfx.hpp"
#include "types.hpp"
#include <getopt.h>
#include <string>

using std::string;

struct runtime_config {
	std::string profile{""};
	std::string gfxdef{""};
	s16 subpalette;
	std::string outfile;

	runtime_config() { subpalette = -1; }
};

class gfxprofile : public chrgfx::gfxdef
{
public:
	gfxprofile(std::string const &id, std::string const &chrdef_id,
						 std::string const &coldef_id, std::string const &paldef_id)
			: chrgfx::gfxdef(std::move(id)), chrdef_id(std::move(chrdef_id)),
				coldef_id(std::move(coldef_id)), paldef_id(std::move(paldef_id)){};

	std::string get_chrdef_id() const { return chrdef_id; }
	std::string get_coldef_id() const { return coldef_id; }
	std::string get_paldef_id() const { return paldef_id; }

private:
	std::string chrdef_id;
	std::string coldef_id;
	std::string paldef_id;
};

// these are intentionally mutable
extern std::string default_short_opts;

extern std::vector<option> default_long_opts;

std::string ltrim(const std::string &s);
std::string rtrim(const std::string &s);
std::string trim(const std::string &s);

std::pair<std::string, std::string> kvsplit(const std::string &line,
																						const char delim = ' ');

bool process_default_args(runtime_config &cfg, int argc, char **argv);

template <typename T> T sto(std::string const &str);

#endif