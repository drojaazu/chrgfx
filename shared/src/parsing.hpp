/**
 * @file parsing.hpp
 * @author Motoi Productions (Damian Rogers damian@motoi.pro)
 * @brief Text parsing utilities
 *
 * Updates:
 * 20211214 Initial
 * 20220921 Small corrections based on clang-tidy suggestions
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
 * @param str Mutable string
 */
template <typename StringT>
void strip_cr(std::basic_string<StringT> & str)
{
	str.erase(std::remove(str.begin(), str.end(), '\r'), str.end());
}

/**
 * @brief Removes any spaces, tabs or common inivisible control characters on
 * the left side of a string
 *
 * @param s Mutable string
 */
template <typename StringT>
void ltrim(std::basic_string<StringT> & str)
{
	str.erase(0, str.find_first_not_of("\t\n\v\f\r "));
}

/**
 * @brief Removes any spaces, tabs or common inivisible control characters on
 * the right side of a string
 *
 * @param s Mutable string
 */
template <typename StringT>
void rtrim(std::basic_string<StringT> & str)
{
	str.erase(str.find_last_not_of("\t\n\v\f\r ") + 1);
}

/**
 * @brief Removes any spaces or tabs on the left and right side of a string
 *
 * @param s Mutable string
 */
template <typename StringT>
void trim(std::basic_string<StringT> & str)
{
	ltrim(str);
	rtrim(str);
}

/**
 * @brief Converts a string with alphabetic characters to lower case
 *
 * @param s Mutable string
 */
template <typename StringT>
void lower(std::basic_string<StringT> & str)
{
	std::transform(str.begin(), str.end(), str.begin(), ::tolower);
}

/**
 * @brief Converts a string with alphabetic characters to upper case
 *
 * @param s Mutable string
 */
template <typename StringT = char>
void upper(std::basic_string<StringT> & str)
{
	std::transform(str.begin(), str.end(), str.begin(), ::toupper);
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
T sto(std::basic_string<StringT> const & str, int const base = 10)
{
	T value;
	auto ec =
		std::from_chars(str.data(), str.data() + str.size(), value, base).ec;
	if(ec == std::errc::result_out_of_range)
	{
		std::stringstream ss;
		ss << "Input value \"" << str << "\" too large for specified numeric type";
		throw std::out_of_range(ss.str());
	}
	else if(ec == std::errc::invalid_argument)
	{
		std::stringstream ss;
		ss << "Input value \"" << str
			 << "\" could not be parsed to a numeric value";
		throw std::invalid_argument(ss.str());
	}
	return value;
}

template <typename T, typename StringT>
T sto_pos(std::basic_string<StringT> const & str, int const base = 10)
{
	T out { sto<T>(str, base) };
	if(out <= 0)
	{
		throw std::out_of_range("Value must be greater than zero");
	}
	return out;
}

/**
 * @brief Converts a delimited list of numeric values into a vector
 */
template <typename T, typename StringT>
std::vector<T> stovec(
	std::basic_string<StringT> const & str, char const delim = ',')
{
	std::vector<T> out;
	std::istringstream ss { str };
	std::basic_string<StringT> line;

	while(getline(ss, line, delim))
	{
		trim(line);
		out.push_back(sto<T>(line));
	}

	return out;
}

/**
 * @brief Parse a string as a boolean
 * @note Allowed values: "t", "true", "1", "f", "false", "0" (upper or lower
 * case)
 */
template <typename StringT>
bool stob(std::basic_string<StringT> const & str)
{
	std::basic_string<StringT> s_work = str;
	lower(s_work);
	if(s_work == "t" || s_work == "true" || s_work == "1")
		return true;
	if(s_work == "f" || s_work == "false" || s_work == "0")
		return false;
	std::stringstream ss;
	ss << "Value \"" << str << "\" could not be interpreted as a boolean";
	throw std::invalid_argument(ss.str());
}

/**
 * @brief Split a string on the specified char delimiter
 * @note Assumes input string has already been whitespace trimmed
 */
template <typename StringT>
std::pair<std::basic_string<StringT>, std::basic_string<StringT>> kvsplit(
	std::basic_string<StringT> const & line, char const delim = ' ')
{
	auto delim_pos { line.find(delim) };
	if(delim_pos == std::string::npos)
	{
		std::stringstream ss;
		ss << "Delimiter '" << delim << "' not found in string \"" << line << "\"";
		throw std::invalid_argument(ss.str());
	}
	return { line.substr(0, delim_pos),
		line.substr(delim_pos + 1, std::string::npos) };
}

#endif
