#ifndef CHRGFX__SHARED_SHARED_HPP
#define CHRGFX__SHARED_SHARED_HPP

#include <getopt.h>
#include <string>
#include <vector>

#include "usage.hpp"

// these are intentionally mutable
extern std::string short_opts;
extern std::vector<option> long_opts;
extern std::vector<motoi::option_details> opt_details;

struct runtime_config
{
	std::string profile_id;
	std::string gfxdefs_path;
	std::string chrdef_id;
	std::string coldef_id;
	std::string paldef_id;

	std::string chrdef_width;
	std::string chrdef_height;
	std::string chrdef_bpp;
	std::string chrdef_plane_offsets;
	std::string chrdef_pixel_offsets;
	std::string chrdef_row_offsets;

	std::string paldef_datasize;
	std::string paldef_entry_datasize;
	std::string paldef_length;

	std::string rgbcoldef_big_endian;
	std::string rgbcoldef_rgblayout;
	std::string rgbcoldef_bitdepth;

	bool chrdef_cli_defined() const;

	bool paldef_cli_defined() const;

	bool coldef_cli_defined() const;
};

bool shared_args(char this_opt, runtime_config & cfg);

std::string get_gfxdefs_path();

#endif
