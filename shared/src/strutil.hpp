/**
 * @file strutil.hpp
 * @author Damian Rogers / damian@motoi.pro
 * @brief String manipulation utilities
 * @copyright ©2024 Motoi Productions / Released under MIT License
 *
 * Updates:
 * 20211214 Initial
 * 20230215 Addition of to_hex
 */

#ifndef __MOTOI__STRUTIL_HPP
#define __MOTOI__STRUTIL_HPP

#include <algorithm>
#include <charconv>
#include <iomanip>
#include <sstream>
#include <string>
#include <type_traits>
#include <vector>

/**
 * @brief Removes a given character from a string
 *
 * @param str Mutable string
 */
template <typename CharT>
inline void strip_char(std::basic_string<CharT> & str, char const c)
{
	str.erase(std::remove(str.begin(), str.end(), c), str.end());
}

/**
 * @brief Removes the \r (CR) character from a string
 *
 * @param str Mutable string
 */
template <typename CharT>
inline void strip_cr(std::basic_string<CharT> & str)
{
	strip_char(str, '\r');
}

/**
 * @brief Remove non-printable control characters from a string
 *
 * @tparam CharT
 * @param str Mutable string
 */
template <typename CharT>
inline void strip_ctrl(std::basic_string<CharT> & str)
{
	str.erase(std::remove_if(str.begin(), str.end(), [](char c) { return std::iscntrl(c); }), str.end());
}

/**
 * @brief Removes any spaces, tabs or common inivisible control characters on
 * the left side of a string
 *
 * @param str Mutable string
 */
template <typename CharT>
inline void ltrim(std::basic_string<CharT> & str)
{
	str.erase(0, str.find_first_not_of("\t\n\v\f\r "));
}

/**
 * @brief Removes any spaces, tabs or common inivisible control characters on
 * the right side of a string
 *
 * @param str Mutable string
 */
template <typename CharT>
inline void rtrim(std::basic_string<CharT> & str)
{
	str.erase(str.find_last_not_of("\t\n\v\f\r ") + 1);
}

/**
 * @brief Removes any spaces or tabs on the left and right side of a string
 *
 * @param str Mutable string
 */
template <typename CharT>
inline void trim(std::basic_string<CharT> & str)
{
	ltrim(str);
	rtrim(str);
}

/**
 * @brief Converts alphabetic characters within a string to lower case
 *
 * @param str Mutable string
 */
template <typename CharT>
inline void lower(std::basic_string<CharT> & str)
{
	std::transform(str.begin(), str.end(), str.begin(), ::tolower);
}

/**
 * @brief Converts alphabetic characters within a string to upper case
 *
 * @param s Mutable string
 */
template <typename CharT>
inline void upper(std::basic_string<CharT> & str)
{
	std::transform(str.begin(), str.end(), str.begin(), ::toupper);
}

/**
 * @brief Converts a string to an arbitrary numeric type
 *
 * @note Uses @c std::from_chars, please see documentation on this function
 * for details regarding valid input patterns
 *
 * @tparam T numeric type
 * @param s string to interpret
 * @param base interpret number with the given base
 * @return T value as a numeric type
 */
template <typename T, typename CharT>
T sto(std::basic_string<CharT> const & str, int const base = 10)
{
	T value;
	auto ec = std::from_chars(str.data(), str.data() + str.size(), value, base).ec;
	if (ec == std::errc::result_out_of_range)
	{
		std::stringstream ss;
		ss << "Input value \"" << str << "\" too large for specified numeric type";
		throw std::out_of_range(ss.str());
	}
	else if (ec == std::errc::invalid_argument)
	{
		std::stringstream ss;
		ss << "Input value \"" << str << "\" could not be parsed to a numeric value";
		throw std::invalid_argument(ss.str());
	}
	return value;
}

template <typename T>
T sto(char const * str, int const base = 10)
{
	return sto<T>(std::string(str), base);
}

/**
 * @brief Converts a string to an arbitrary numeric type; throws an exception if the value is negative
 *
 * @note Uses @c std::from_chars, please see documentation on this function
 * for details regarding valid input patterns
 *
 * @tparam T numeric type
 * @param s string to interpret
 * @param base interpret number with the given base
 * @return T value as a numeric type
 */
template <typename T, typename CharT>
T sto_pos(std::basic_string<CharT> const & str, int const base = 10)
{
	T out {sto<T>(str, base)};
	if (out <= 0)
		throw std::out_of_range("Value must be greater than zero");
	return out;
}

template <typename T>
T sto_pos(char const * str, int const base = 10)
{
	return sto_pos<T>(std::string(str), base);
}

/**
 * @brief
 *
 * @tparam ContainerT Container type with push_back method
 * @tparam CharT
 * @param str
 * @param delim
 * @return ContainerT
 */
template <typename ContainerT, typename CharT>
ContainerT sto_container(std::basic_string<CharT> const & str, char const delim = ',')
{
	ContainerT out;
	std::istringstream ss {str};
	std::basic_string<CharT> line;

	while (getline(ss, line, delim))
	{
		trim(line);
		out.push_back(sto<typename ContainerT::value_type>(line));
	}

	return out;
}

/**
 * @brief
 *
 * @tparam ContainerT Container type with push_back method and element count constructor
 * @tparam CharT
 * @tparam std::enable_if<std::is_integral<typename ContainerT::value_type>::value, ContainerT>::type
 * @param str
 * @param delim
 * @return ContainerT
 */
template <typename ContainerT,
	typename CharT,
	typename = typename std::enable_if<std::is_integral<typename ContainerT::value_type>::value, ContainerT>::type>
ContainerT sto_range_by_count(std::basic_string<CharT> const & str, char const delim = ',')
{
	if (str[0] != '[' || str[str.size() - 1] != ']')
		throw std::runtime_error("invalid range specification");
	std::string work_str {str.begin() + 1, str.end() - 1};
	using int_t = typename ContainerT::value_type;
	auto vals = sto_container<std::vector<int_t>>(work_str, delim);
	int_t start, count, step;
	switch (vals.size())
	{
		case 1:
			start = 0;
			count = vals[0];
			step = 1;
			break;
		case 2:
			start = vals[0];
			count = vals[1];
			step = 1;
			break;
		case 3:
			start = vals[0];
			count = vals[1];
			step = vals[2];
			break;
		default:
			throw std::runtime_error("invalid range specification");
	}

	ContainerT out;
	for (int_t iter {0}, val {start}; iter < count; ++iter, val += step)
		out.push_back(val);

	return out;
}

/**
 * @brief Parse a string as a boolean
 * @note Allowed values: "t", "true", "1" for true, "f", "false", "0" for false(upper or lower
 * case)
 */
template <typename CharT>
bool sto_bool(std::basic_string<CharT> const & str)
{
	std::basic_string<CharT> s_work = str;
	lower(s_work);
	if (s_work == "t" || s_work == "true" || s_work == "1")
		return true;
	if (s_work == "f" || s_work == "false" || s_work == "0")
		return false;
	std::ostringstream oss;
	oss << "Value \"" << str << "\" could not be interpreted as a boolean";
	throw std::invalid_argument(oss.str());
}

template <typename CharT>
std::basic_string<CharT> to_hex(const char * data, size_t const len)
{
	std::basic_ostringstream<CharT> oss;
	// TODO: do we need to set hex/fill/width on each iter?
	for (size_t iter = 0; iter < len; ++iter)
		oss << std::hex << std::setfill('0') << std::setw(2) << (int) (unsigned char) data[iter];
	return oss.str();
}

#endif
