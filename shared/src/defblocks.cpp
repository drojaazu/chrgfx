
#include "defblocks.hpp"
#include <algorithm>
#include <cerrno>
#include <cstring>
#include <fstream>
#include <iostream>
#include <vector>

using namespace std;

char constexpr COMMENT_MARKER { '#' };
char constexpr BLOCK_OPENER { '{' };
char constexpr BLOCK_CLOSER { '}' };
char constexpr KV_DELIM { ' ' };

defblock_key_error::defblock_key_error(char const * what, char const * key,
																			 char const * block_id) :
		m_key(key),
		m_block_id(block_id), std::runtime_error(what)
{
}

char const * defblock_key_error::key() const
{
	return m_key;
}

char const * defblock_key_error::block() const
{
	return m_block_id;
}

defblock_value_error::defblock_value_error(char const * what, char const * key,
																					 char const * value,
																					 char const * block_id) :
		defblock_key_error(what, key, block_id),
		m_value(value)
{
}

char const * defblock_value_error::value() const
{
	return m_value;
}

string ltrim(string const & s)
{
	size_t start = s.find_first_not_of(' ');
	return (start == string::npos) ? "" : s.substr(start);
}

string rtrim(string const & s)
{
	size_t end = s.find_last_not_of(' ');
	return (end == string::npos) ? "" : s.substr(0, end + 1);
}

string trim(string const & s)
{
	return rtrim(ltrim(s));
}

// assumes the string passed in has already had any whitespace trimmed
pair<string, string> kvsplit(string const & line)
{
	std::size_t spacedelim_pos;
	spacedelim_pos = line.find(KV_DELIM);
	if(spacedelim_pos == string::npos)
	{
		throw std::invalid_argument(
				"Could not find specified delimiter in given string");
	}
	return pair<string, string> { line.substr(0, spacedelim_pos),
																line.substr(spacedelim_pos + 1, string::npos) };
}

defblock parse_defblock(std::istream & in)
{
	if(!in.good())
		throw runtime_error("Could not read from defblock stream");

	string opener_check;
	std::getline(in, opener_check);
	std::replace(opener_check.begin(), opener_check.end(), '\t', ' ');
	opener_check = trim(opener_check);

	if(opener_check[0] != BLOCK_OPENER || opener_check.size() != 1)
		throw runtime_error("Block opener not found after definition");

	string this_line;

	defblock out;

	while(std::getline(in, this_line))
	{
		std::replace(this_line.begin(), this_line.end(), '\t', ' ');
		this_line = trim(this_line);
		if(this_line == "" || this_line[0] == COMMENT_MARKER)
		{
			// ignore comment & empty lines
			continue;
		}

		// end of block
		if(this_line[0] == BLOCK_CLOSER)
		{

			break;
		}

		// didn't find a closer
		if(in.eof())
		{
			throw "Reached end of input before finding a block closer";
		}

		// no issues, just a normal line, clean and append to the block
		out.insert(kvsplit(this_line));
	}

	return out;
}

std::multimap<string const, defblock const> load_defblocks(istream & in)
{
	if(!in.good())
		throw runtime_error("Could not read from defblock stream");

	string this_line, work_line, cached_line, this_def_name, this_block;

	multimap<string const, defblock const> out;

	while(getline(in, this_line))
	{
		work_line = this_line;
		replace(work_line.begin(), work_line.end(), '\t', ' ');
		work_line = trim(this_line);

		// blank line or a comment; move on
		if(work_line.empty() || work_line[0] == COMMENT_MARKER)
		{
			continue;
		}

		// we shouldn't expect to find a block opener/closer at this point
		// (should be on the next line)
		if(work_line.find(BLOCK_OPENER) != string::npos ||
			 work_line.find(BLOCK_CLOSER) != string::npos)
		{
			std::cerr << "Found block delimiter outside block declaration"
								<< std::endl;
			continue;
		}

		// assume any text we've found is a block declaration
		this_def_name = rtrim(work_line);
		// stream pointer should now be ready at the next line
		try
		{
			defblock this_defblock = parse_defblock(in);
			out.insert({ this_def_name, this_defblock });
		}
		catch(string const & e)
		{
			std::cerr << e << std::endl;
			continue;
		}
	}
	return out;
}
