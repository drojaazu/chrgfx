#ifndef CHRGFX_EXEC_IMPORT_DEFS_H
#define CHRGFX_EXEC_IMPORT_DEFS_H

#include <algorithm>
#include <array>
#include <cerrno>
#include <fstream>
#include <map>
#include <sstream>
#include <string.h>
#include <string>
#include <tuple>
#include <vector>

#include "chrgfx.hpp"
#include "defblocks.hpp"
#include "gfxprofile.hpp"
#include "shared.hpp"
#include "vd.hpp"

struct def_collection
{
public:
	std::map<string const, chrgfx::chrdef const> const chrdefs;
	std::map<string const, chrgfx::rgbcoldef const> const rgbcoldefs;
	std::map<string const, chrgfx::refcoldef const> const refcoldefs;
	std::map<string const, chrgfx::paldef const> const paldefs;
	std::map<string const, gfxprofile const> const profiles;
};

png::palette parse_palette(std::string const & pal);

def_collection load_gfxdefs(string const & def_file);

chrgfx::chrdef validate_chrdef_block(defblock const & def_block);

chrgfx::rgbcoldef validate_rgbcoldef_block(defblock const & def_block);

chrgfx::refcoldef validate_refcoldef_block(defblock const & def_block);

chrgfx::paldef validate_paldef_block(defblock const & def_block);

gfxprofile validate_profile_block(defblock const & def_block);

#endif