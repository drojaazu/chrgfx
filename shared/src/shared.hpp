#ifndef CHRGFX__SHARED_SHARED_HPP
#define CHRGFX__SHARED_SHARED_HPP

#include <getopt.h>
#include <iostream>
#include <string>
#include <vector>

#include "builtin_defs.hpp"
#include "chrdef.hpp"
#include "coldef.hpp"
#include "import_defs.hpp"
#include "paldef.hpp"
#include "usage.hpp"

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

	std::string chrdef_width;
	std::string chrdef_height;
	std::string chrdef_bpp;
	std::string chrdef_plane_offsets;
	std::string chrdef_pixel_offsets;
	std::string chrdef_row_offsets;

	bool list_gfxdefs {false};
};

class def_helper
{
public:
	chrgfx::chrdef chrdef {chrgfx::gfxdefs::basic_8x8_1bpp};
	chrgfx::coldef coldef {chrgfx::gfxdefs::basic_8bit_random};
	chrgfx::paldef paldef {chrgfx::gfxdefs::basic_256color};

	def_helper(runtime_config & cfg);

	void list_gfxdefs(std::ostream & os);

private:
	def_collection m_defs;
};

bool shared_args(char this_opt, runtime_config & cfg);

#endif
