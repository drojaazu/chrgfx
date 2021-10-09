#ifndef CHRGFX_EXEC_IMPORT_DEFS_H
#define CHRGFX_EXEC_IMPORT_DEFS_H

#include <algorithm>
#include <array>
#include <cerrno>
#include <fstream>
#include <map>
#include <sstream>
#include <string>
#include <tuple>
#include <vector>

#include "builtin_defs.hpp"
#include "chrgfx.hpp"
#include "defblocks.hpp"
#include "shared.hpp"
#include "vd.hpp"

class gfxdef_key_error : public std::runtime_error
{
public:
	gfxdef_key_error(std::string err, std::string key,
									 std::string block_id = {}) :
			err(std::move(err)),
			key(std::move(key)), block_id(std::move(block_id)),
			std::runtime_error(
					"Error parsing gfxdef file: '" + err + "' for key '" + key + "'" +
					(block_id.empty() ? std::string {}
														: " in block '" + block_id + "'")) {};

private:
	std::string key;
	std::string block_id;
	std::string err;
};

class gfxdef_value_error : public std::runtime_error
{
public:
	gfxdef_value_error(std::string err, std::string val, std::string key,
										 std::string block_id = {}) :
			err(std::move(err)),
			val(val.length() > 16 ? (val.substr(0, 15) + "...") : std::move(val)),
			key(std::move(key)), block_id(std::move(block_id)),
			std::runtime_error("Error parsing gfxdef file: '" + err +
												 "' error for val '" + val + "' on key '" + key + "'" +
												 (block_id.empty()
															? std::string {}
															: " in block '" + block_id + "'")) {};

private:
	std::string key;
	std::string val;
	std::string block_id;
	std::string err;
};

png::palette create_palette(std::string const & pal);

std::tuple<std::map<string const, chrgfx::chrdef const>,
					 std::map<string const, chrgfx::rgbcoldef const>,
					 std::map<string const, chrgfx::refcoldef const>,
					 std::map<string const, chrgfx::paldef const>,
					 std::map<string const, gfxprofile const>>
load_gfxdefs(string const & def_file);

chrgfx::chrdef validate_chrdef_block(defblock const & def_block);

chrgfx::rgbcoldef validate_rgbcoldef_block(defblock const & def_block);

chrgfx::refcoldef validate_refcoldef_block(defblock const & def_block);

chrgfx::paldef validate_paldef_block(defblock const & def_block);

gfxprofile validate_profile_block(defblock const & def_block);

#endif