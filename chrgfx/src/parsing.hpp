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

#include <algorithm>
#include <charconv>
#include <sstream>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

/**
 * @brief Removes the \r (CR) character from a string
 *
 * @param s Mutable string
 */
template <typename StringT>
void strip_cr (std::basic_string<StringT> & s)
{
	s.erase (std::remove (s.begin (), s.end (), '\r'), s.end ());
}

/**
 * @brief Removes any spaces, tabs or common inivisible control characters on
 * the left side of a string
 *
 * @param s Mutable string
 */
template <typename StringT>
void ltrim (std::basic_string<StringT> & s)
{
	s.erase (0, s.find_first_not_of ("\t\n\v\f\r "));
}

/**
 * @brief Removes any spaces, tabs or common inivisible control characters on
 * the right side of a string
 *
 * @param s Mutable string
 */
template <typename StringT>
void rtrim (std::basic_string<StringT> & s)
{
	s.erase (s.find_last_not_of ("\t\n\v\f\r ") + 1);
}

/**
 * @brief Removes any spaces or tabs on the left and right side of a string
 *
 * @param s Mutable string
 */
template <typename StringT>
void trim (std::basic_string<StringT> & s)
{
	ltrim (s);
	rtrim (s);
}

/**
 * @brief Converts a string with alphabetic characters to lower case
 *
 * @param s Mutable string
 */
template <typename StringT>
void lower (std::basic_string<StringT> & s)
{
	std::transform (s.begin (), s.end (), s.begin (), ::tolower);
}

/**
 * @brief Converts a string with alphabetic characters to upper case
 *
 * @param s Mutable string
 */
template <typename StringT = char>
void upper (std::basic_string<StringT> & s)
{
	std::transform (s.begin (), s.end (), s.begin (), ::toupper);
}

/**
 * @brief Converts a string to an arbitrary numeric type
 *
 * @note Uses @c std::from_chars, please see documentation on this function
 * for details about valid input patterns
 *
 * @tparam T numeric type
 * @param s string to interpret
 * @param base interpret number with the given base
 * @return T value as a numeric type
 */
template <typename T, typename StringT>
T sto (std::basic_string<StringT> const & s, int const base = 10)
{
	T value;
	auto ec = std::from_chars (s.data (), s.data () + s.size (), value, base).ec;
	if (ec == std::errc::result_out_of_range)
	{
		std::stringstream ss;
		ss << "Input value \"" << s << "\" too large for specified numeric type";
		throw std::out_of_range (ss.str ());
	}
	else if (ec == std::errc::invalid_argument)
	{
		std::stringstream ss;
		ss << "Input value \"" << s << "\" could not be parsed to a numeric value";
		throw std::invalid_argument (ss.str ());
	}
	return value;
}

template <typename T, typename StringT>
T sto_pos (std::basic_string<StringT> const & s, int const base = 10)
{
	T out {sto<T> (s)};
	if (out <= 0)
	{
		throw std::out_of_range ("Value must be greater than zero");
	}
	return out;
}

/**
 * @brief Converts a delimited list of numeric values into a vector
 */
template <typename T, typename StringT>
std::vector<T> stovec (std::basic_string<StringT> const & s, char const delim = ',')
{
	std::vector<T> out;
	std::istringstream ss {s};
	std::basic_string<StringT> line;

	while (getline (ss, line, delim))
	{
		trim (line);
		out.push_back (sto<T> (line));
	}

	return out;
}

/**
 * @brief Parse a string as a boolean
 * @note Allowed values: "t", "true", "1", "f", "false", "0" (upper or lower
 * case)
 */
template <typename StringT>
bool stob (std::basic_string<StringT> const & s)
{
	std::basic_string<StringT> s_work = s;
	lower (s_work);
	if (s_work == "t" || s_work == "true" || s_work == "1")
		return true;
	if (s_work == "f" || s_work == "false" || s_work == "0")
		return false;
	std::stringstream ss;
	ss << "Value \"" << s << "\" could not be interpreted as a boolean";
	throw std::invalid_argument (ss.str ());
}

/**
 * @brief Split a string on the specified char delimiter
 * @note Assumes input string has already been whitespace trimmed
 */
template <typename StringT>
std::pair<std::basic_string<StringT>, std::basic_string<StringT>> kvsplit (
	std::basic_string<StringT> const & line, char const delim = ' ')
{
	size_t delim_pos;
	delim_pos = line.find (delim);
	if (delim_pos == std::string::npos)
	{
		std::stringstream ss;
		ss << "Delimiter '" << delim << "' not found in string \"" << line << "\"";
		throw std::invalid_argument (ss.str ());
	}
	return {line.substr (0, delim_pos), line.substr (delim_pos + 1, std::string::npos)};
}

#endif
