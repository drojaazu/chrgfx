#ifndef CHRGFX_EXEC_SHARED_H
#define CHRGFX_EXEC_SHARED_H

#include "types.hpp"
#include <getopt.h>
#include <string>

class gfxprofile : public chrgfx::gfx_def
{
public:
	gfxprofile(std::string const &id, std::string const &chrdef_id,
						 std::string const &coldef_id, std::string const &paldef_id);

	std::string get_chrdef_id();
	std::string get_coldef_id();
	std::string get_paldef_id();
};

// these are intentionally mutable
extern std::string default_short_opts;

extern std::vector<option> default_long_opts;

struct runtime_config {
	std::string gfxdef_name{""}, chrdef_name{""}, paldef_name{""};

	// std::string chrdata_name{""}, paldata_name{""}, pngdata_name{""};

	s16 subpalette;

	std::string outfile;

	runtime_config() { subpalette = -1; }
};

std::string ltrim(const std::string &s);
std::string rtrim(const std::string &s);
std::string trim(const std::string &s);

std::pair<std::string, std::string> kvsplit(const std::string &line,
																						const char delim = ' ');

bool process_default_args(runtime_config &cfg, int argc, char **argv);

template <typename T> T sto(const std::string &str);

#endif