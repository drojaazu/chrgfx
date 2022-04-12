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

class gfxdef_key_error : public std::exception
{
public:
	gfxdef_key_error(char const * err, char const * key,
									 char const * block_id = nullptr);

	const char * what() const noexcept override;

private:
	char const * m_key;
	char const * m_block_id;
	char const * m_err;
};

class gfxdef_value_error : public std::exception
{
public:
	gfxdef_value_error(char const * err, char const * key, char const * value,
										 char const * block_id = nullptr);

	const char * what() const noexcept override;

private:
	char const * m_key;
	char const * m_value;
	char const * m_block_id;
	char const * m_err;
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