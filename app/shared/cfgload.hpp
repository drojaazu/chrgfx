/**
 * @file cfgload2.hpp
 * @author Damian Rogers / damian@motoi.pro
 * @brief Reads and parses a text file containing configuration data in a simple format of a header with
 *        key/value pairs listed in a brace block.
 * @copyright ©2024 Motoi Productions / Released under MIT License
 *
 * Updates:
 * 20241205 Initial
 */

#ifndef __MOTOI__CFGLOAD_HPP
#define __MOTOI__CFGLOAD_HPP

#include "lineread.hpp"
#include <map>
#include <stdexcept>
#include <string>

/*
Example config block:

# This is a comment
config_block
{
	some_value 1234
	some_filepath /usr/local/whatever
}

This is config block which is composed of config entries. Each entry is a simple key/value pair
seperated by a space OR tab. This means that keys cannot have spaces/tabs.
(Block headers may have spaces, but they should probably be avoided for the sake of consistency with entries.)

a block header should appear first, following by open bracket { on the next line, a number of config entries,
and then a closing bracket } on the line after that.

Comments are prefixed with a hash #

Whitespace (matching "\t\n\v\f\r ") is trimmed before processing, so entries or blocks can be indented
as preferred by the user.

Nested blocks are not supported.

No type or data checking is done. Each entry is stored as an unordered multimap of <string, string>, while the
whole config is stored as an unordered multimap of <string, block map> where each string is a block header.
*/

namespace motoi
{
class file_parse_error : public std::runtime_error
{
private:
	std::string m_err_msg;
	size_t m_line_number;

public:
	file_parse_error() = delete;
	file_parse_error(char const * message, size_t line_number);
};

using block_map = std::multimap<std::string, std::string>;
using config_map = std::multimap<std::string, block_map>;

class config_loader : public config_map
{
private:
	static auto constexpr COMMENT_MARKER {'#'};
	static auto constexpr BLOCK_OPENER {'{'};
	static auto constexpr BLOCK_CLOSER {'}'};

	motoi::linereader<char> m_file;

	enum class parse_state : uint8_t
	{
		search_header,
		search_opener,
		search_closer,
		in_block
	};

	parse_state m_parse_state;

public:
	config_loader(std::string const & filepath);
	config_loader() = delete;
};
} // namespace motoi

#endif