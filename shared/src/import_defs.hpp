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

class gfxdef_key_error : public std::exception
{
public:
	gfxdef_key_error(std::string err, std::string key, std::string block_id = {})
			: err(std::move(err)), key(std::move(key)), block_id(std::move(block_id)),
				msg("'" + err + "' for key '" + key + "'" +
						(block_id.empty() ? std::string{}
															: " in block '" + block_id + "'")){};
	const char *what() { return msg.c_str(); };

private:
	std::string msg;
	std::string key;
	std::string block_id;
	std::string err;
};

class gfxdef_value_error : public std::exception
{
public:
	gfxdef_value_error(std::string err, std::string val, std::string key,
										 std::string block_id = {})
			: err(std::move(err)),
				val(val.length() > 16 ? (val.substr(0, 15) + "...") : std::move(val)),
				key(std::move(key)), block_id(std::move(block_id)),
				msg("'" + err + "' for val '" + val + "' on key '" + key + "'" +
						(block_id.empty() ? std::string{}
															: " in block '" + block_id + "'")){};
	const char *what() { return msg.c_str(); };

private:
	std::string msg;
	std::string key;
	std::string val;
	std::string block_id;
	std::string err;
};

png::palette create_palette(std::string const &pal);

std::tuple<map<string const, chrgfx::chrdef const>,
					 map<string const, chrgfx::coldef const>,
					 map<string const, chrgfx::paldef const>,
					 map<string const, gfxprofile const>>
load_gfxdefs(string const &def_file);

chrgfx::chrdef validate_chrdef_block(defblock const &def_block);

chrgfx::coldef validate_coldef_block(defblock const &def_block);

chrgfx::paldef validate_paldef_block(defblock const &def_block);

gfxprofile validate_profile_block(defblock const &def_block);

#endif