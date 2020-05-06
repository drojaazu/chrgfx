#ifndef CHRGFX_EXEC_IMPORT_DEFS_H
#define CHRGFX_EXEC_IMPORT_DEFS_H

#include "chrgfx.hpp"
#include "shared.hpp"
#include <map>
#include <string>
#include <tuple>

using std::map;
using std::string;
using namespace chrgfx;

std::tuple<map<string const, chr_def const>, map<string const, col_def const>,
					 map<string const, pal_def const>,
					 map<string const, gfxprofile const>>
load_gfxdefs(string const &def_file);

#endif