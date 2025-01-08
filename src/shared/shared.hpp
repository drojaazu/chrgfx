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
extern std::string const GFXDEF_SUBDIR;

// these are intentionally mutable
extern std::string short_opts;
extern std::vector<option> long_opts;
extern std::vector<option_details> opt_details;

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

	std::string rgbcoldef_big_endian;
	std::string rgbcoldef_rgblayout;
	std::string rgbcoldef_bitdepth;
};

class def_helper
{
public:
	chrgfx::chrdef const * chrdef {&chrgfx::gfxdefs::chr_8x8_4bpp_packed_lsb};
	chrgfx::coldef const * coldef {&chrgfx::gfxdefs::col_bgr_333_packed};
	chrgfx::paldef const * paldef {&chrgfx::gfxdefs::pal_16bit_256color};

	def_helper(runtime_config & cfg);

	~def_helper();

	void list_gfxdefs(std::ostream & os);

private:
	def_collection m_defs;
	bool m_heapallocated_chrdef {false};
	bool m_heapallocated_coldef {false};
	bool m_heapallocated_paldef {false};

	gfxprofile const & find_gfxprofile(std::string const & gfxprofile_id);
	chrgfx::chrdef const * find_chrdef(std::string const & chrdef_id);
	chrgfx::coldef const * find_coldef(std::string const & coldef_id);
	chrgfx::paldef const * find_paldef(std::string const & paldef_id);
	bool use_chrdefbuilder(runtime_config & cfg);
	bool use_rgbcoldefbuilder(runtime_config & cfg);
};

bool shared_args(char this_opt, runtime_config & cfg);

string get_gfxdefs_path();

#endif
