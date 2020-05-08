#ifndef CHRGFX_EXEC_IMPORT_DEFS_H
#define CHRGFX_EXEC_IMPORT_DEFS_H

#include "chrgfx.hpp"
#include "shared.hpp"
#include <algorithm>
#include <array>
#include <bits/stdc++.h>
#include <cerrno>
#include <fstream>
#include <map>
#include <sstream>
#include <string>
#include <tuple>
#include <vector>

#include "chrgfx.hpp"
#include "defblocks.hpp"
#include "shared.hpp"
#include "vd.hpp"

using std::map;
using std::string;
using namespace chrgfx;

std::tuple<map<string const, chr_def const>, map<string const, col_def const>,
					 map<string const, pal_def const>,
					 map<string const, gfxprofile const>>
load_gfxdefs(string const &def_file);

chr_def validate_chrdef_block(defblock const &def_block);
palette create_palette(std::string const &pal);

col_def validate_coldef_block(defblock const &def_block);

pal_def validate_paldef_block(defblock const &def_block);

gfxprofile validate_profile_block(defblock const &def_block);

#endif