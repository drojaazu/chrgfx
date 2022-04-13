/**
 * @file parsing.hpp
 * @author Motoi Productions (Damian Rogers damian@motoi.pro)
 * @brief Text parsing utilities
 *
 * Updates:
 * 20211214 Initial
 */

#ifndef __MOTOI__PARSING_HPP
#define __MOTOI__PARSING_HPP

#include <charconv>
#include <sstream>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <vector>

using std::errc;
using std::istringstream;
using std::string;
using std::vector;

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

/**
 * @brief Converts a delimited list of integer values into a vector
 */
template <typename T = ushort>
vector<T> sto_vector(char const * val, char const delim = ',')
{
	vector<T> out;
	istringstream ss {val};
	string line;

	while(getline(ss, line, delim))
	{
		T value;
		auto ec = from_chars(line.data(), line.data() + line.size(), value).ec;
		if(ec == errc::result_out_of_range)
		{
			istringstream ss;
			ss << "Parsed value \"" << line << "\" too large for numeric type";
			throw std::out_of_range(ss.str());
		}
		else if(ec == errc::invalid_argument)
		{
			istringstream ss;
			ss << "Parsed value \"" << line
				 << "\" could not be evaluated to a numeric value";
			throw std::invalid_argument(ss.str());
		}

		out.push_back(value);
	}

	return out;
}

/**
 * @brief Split a string on the specified char delimiter
 * @note Assumes input string has already been whitespace trimmed
 */
pair<string, string> kvsplit(string const & line, char const delim = ' ')
{
	size_t spacedelim_pos;
	spacedelim_pos = line.find(delim);
	if(spacedelim_pos == string::npos)
	{
		stringstream ss;
		ss << "Delimited '" << delim << "' not found in string \"" << line << "\"";
		throw std::invalid_argument(ss.str());
	}
	return pair<string, string> {line.substr(0, spacedelim_pos),
															 line.substr(spacedelim_pos + 1, string::npos)};
}

#endif