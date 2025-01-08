
#include "defblocks.hpp"
#include "strutil.hpp"
#include <algorithm>
#include <cstring>
#include <iostream>

using namespace std;
using namespace motoi;

/**
 * @brief Split a string on the specified char delimiter
 * @note Assumes input string has already been whitespace trimmed
 */
/*
template <typename StringT>
std::pair<std::basic_string<StringT>, std::basic_string<StringT>> kvsplit(
 std::basic_string<StringT> const & line, char const delim = ' ')
{
 auto delim_pos {line.find(delim)};
 if (delim_pos == std::string::npos)
 {
	 std::stringstream ss;
	 ss << "Delimiter '" << delim << "' not found in string \"" << line << "\"";
	 throw std::invalid_argument(ss.str());
 }
 return {line.substr(0, delim_pos), line.substr(delim_pos + 1, std::string::npos)};
}
*/

char constexpr COMMENT_MARKER {'#'};
char constexpr BLOCK_OPENER {'{'};
char constexpr BLOCK_CLOSER {'}'};
char constexpr KV_DELIM {' '};

defblock_key_error::defblock_key_error(string const & what, string const & key) :
		runtime_error(what + " (" + key + ")"),
		m_key(key)
{
}

string defblock_key_error::key() const
{
	return m_key;
}

defblock_value_error::defblock_value_error(string const & what, string const & key, string const & value) :
		runtime_error(what + " (" + key + " : " + value + ") "),
		m_value(value)
{
}

string defblock_value_error::key() const
{
	return m_key;
}

string defblock_value_error::value() const
{
	return m_value;
}

enum class BlockParseState
{
	BlockName,
	BlockOpener,
	BlockContent,
	BlockCloser
};

defblock parse_defblock_state_machine(istream & in)
{
	if (! in.good() || in.eof())
		throw runtime_error("Could not read from defblock stream");

	BlockParseState parse_state {BlockParseState::BlockName};
	defblock out;
	string this_line;
	while (getline(in, this_line))
	{
		switch (parse_state)
		{
			case BlockParseState::BlockName:
				break;
			case BlockParseState::BlockOpener:
				break;
			case BlockParseState::BlockContent:
				break;
			case BlockParseState::BlockCloser:
				break;
		}
	}
}

defblock parse_defblock(istream & in)
{
	if (! in.good())
		throw runtime_error("Could not read from defblock stream");

	string opener_check;
	getline(in, opener_check);
	replace(opener_check.begin(), opener_check.end(), '\t', ' ');
	trim(opener_check);

	if (opener_check[0] != BLOCK_OPENER || opener_check.size() != 1)
		throw runtime_error("Block opener not found after definition");

	string this_line;

	defblock out;

	while (getline(in, this_line))
	{
		replace(this_line.begin(), this_line.end(), '\t', ' ');
		trim(this_line);
		if (this_line.empty() || this_line[0] == COMMENT_MARKER)
		{
			// ignore comment & empty lines
			continue;
		}

		// end of block
		if (this_line[0] == BLOCK_CLOSER)
		{

			break;
		}

		// didn't find a closer
		if (in.eof())
		{
			throw "Reached end of input before finding a block closer";
		}

		// no issues, just a normal line, clean and append to the block
		out.insert(kvsplit(this_line));
	}

	return out;
}

multimap<string const, defblock const> load_defblocks(istream & in)
{
	if (! in.good())
		throw runtime_error("Could not read from defblock stream");

	string this_line, cached_line, this_def_name, this_block;

	multimap<string const, defblock const> out;

	size_t line_num = 1;

	while (getline(in, this_line))
	{
		trim(this_line);

		// blank line or a comment; move on
		if (this_line.empty() || this_line[0] == COMMENT_MARKER)
			continue;

		// we shouldn't expect to find a block opener/closer at this point
		// (should be on the next line)
		if (this_line.find(BLOCK_OPENER) != string::npos || this_line.find(BLOCK_CLOSER) != string::npos)
		{
			stringstream ss;
			ss << "Delimiter found in block name on line " << line_num;
			throw runtime_error(ss.str());
		}

		// the text we've found so far should be the name of the a block
		// proceed with processing the next line as a block
		defblock this_defblock = parse_defblock(in);
		out.insert({this_line, this_defblock});

		++line_num;
	}
	return out;
}
