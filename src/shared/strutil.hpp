/**
 * @file strutil.hpp
 * @author Damian Rogers / damian@motoi.pro
 * @brief String manipulation utilities
 * @copyright ©2024 Motoi Productions / Released under MIT License
 *
 * Updates:
 * 20211214 Initial
 * 20230215 Addition of to_hex
 * 20250103 Added string_view to a number of functions; added sto_array; added split_container; added split_array
 */

#ifndef __MOTOI__STRUTIL_HPP
#define __MOTOI__STRUTIL_HPP

#include <algorithm>
#include <array>
#include <charconv>
#include <iomanip>
#include <sstream>
#include <string>
#include <type_traits>
#include <vector>

namespace motoi
{
/**
 * @brief Removes all instances of a given character from a string
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

constexpr auto WHITESPACE {"\t\n\v\f\r "};

template <typename CharT>
inline std::basic_string_view<CharT> trim_view(std::basic_string<CharT> const & str)
{
	return std::string_view(str).substr(str.find_first_not_of(WHITESPACE), str.find_last_not_of(WHITESPACE) + 1);
}

template <typename CharT>
inline std::basic_string_view<CharT> trim_view(std::basic_string_view<CharT> str)
{
	return str.substr(str.find_first_not_of(WHITESPACE), str.find_last_not_of(WHITESPACE) + 1);
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

template <typename NumT, typename CharT>
NumT sto(std::basic_string_view<CharT> const & str, int const base = 10)
{
	NumT value;
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

/**
 * @brief Converts a string to an arbitrary numeric type
 *
 * @note Uses @c std::from_chars, please see documentation on this function
 * for details regarding valid input patterns
 *
 * @tparam NumT numeric type
 * @param s string to interpret
 * @param base interpret number with the given base
 * @return T value as a numeric type
 */
template <typename NumT, typename CharT>
NumT sto(std::basic_string<CharT> const & str, int const base = 10)
{
	return sto<NumT>(std::basic_string_view<CharT>(str), base);
}

template <typename NumT>
NumT sto(char const * str, int const base = 10)
{
	return sto<NumT>(std::string(str), base);
}

/**
 * @brief Converts a string to an arbitrary numeric type; throws an exception if the value is negative
 *
 * @note Uses @c std::from_chars, please see documentation on this function
 * for details regarding valid input patterns
 *
 * @tparam NumT numeric type
 * @param s string to interpret
 * @param base interpret number with the given base
 * @return T value as a numeric type
 */
template <typename NumT, typename CharT>
NumT sto_pos(std::basic_string<CharT> const & str, int const base = 10)
{
	NumT out {sto<NumT>(str, base)};
	if (out <= 0)
		throw std::out_of_range("Value must be greater than zero");
	return out;
}

template <typename NumT>
NumT sto_pos(char const * str, int const base = 10)
{
	return sto_pos<NumT>(std::string(str), base);
}

template <typename ContainerT, typename CharT>
ContainerT split_container(std::basic_string_view<CharT> str, char const delim = ',')
{
	ContainerT out;
	size_t start {0}, len {0};
	for (size_t i {0}; i < str.size(); ++i)
	{
		if (str[i] == delim && len > 0)
		{
			out.push_back(trim_view(std::string_view(str.begin() + start, len)));
			len = 0;
			start = i + 1;
			continue;
		}
		++len;
	}

	if (len > 0)
		out.push_back(trim_view(std::string_view(str.begin() + start, len)));

	return out;
}

template <typename ContainerT, typename CharT>
ContainerT split_container(std::basic_string<CharT> const & str, char const delim = ',')
{
	return split_container<ContainerT>(std::basic_string_view<CharT>(str));
}

template <typename ArrayT, size_t Size, typename CharT>
std::array<ArrayT, Size> split_array(std::basic_string_view<CharT> str, char const delim = ',')
{
	std::array<ArrayT, Size> out;
	size_t start {0}, len {0}, index {0};
	for (size_t i {0}; i < str.size(); ++i)
	{
		if (str[i] == delim && len > 0)
		{
			out[index++] = std::string(trim_view(std::string_view(str.begin() + start, len)));
			len = 0;
			start = i + 1;
			continue;
		}
		++len;
	}

	if (len > 0)
		out[index++] = trim_view(std::string_view(str.begin() + start, len));

	return out;
}

template <typename ArrayT, size_t Size, typename CharT>
std::array<ArrayT, Size> split_array(std::basic_string<CharT> const & str, char const delim = ',')
{
	return split_array<ArrayT, Size>(std::basic_string_view<CharT>(str));
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
ContainerT sto_container(std::basic_string_view<CharT> str, char const delim = ',')
{
	ContainerT out;
	size_t start {0}, len {0};
	for (size_t i {0}; i < str.size(); ++i)
	{
		if (str[i] == delim && len > 0)
		{
			out.push_back(sto<typename ContainerT::value_type>(trim_view(std::string_view(str.begin() + start, len))));
			len = 0;
			start = i + 1;
			continue;
		}
		++len;
	}

	if (len > 0)
		out.push_back(sto<typename ContainerT::value_type>(trim_view(std::string_view(str.begin() + start, len))));

	return out;
}

template <typename ContainerT, typename CharT>
ContainerT sto_container(std::basic_string<CharT> const & str, char const delim = ',')
{
	return sto_container<ContainerT>(std::basic_string_view<CharT>(str), delim);
}

/**
 * @brief
 *
 * @tparam ContainerT Container type with push_back method
 * @tparam NumT Numeric type
 * @tparam Size Size of the array
 * @tparam CharT Char type in string
 * @param str
 * @param delim
 * @return ContainerT
 */
template <typename NumT, size_t Size, typename CharT>
std::array<NumT, Size> sto_array(std::basic_string_view<CharT> str, char const delim = ',')
{
	std::array<NumT, Size> out;
	out.fill(0);
	size_t start {0}, len {0}, index {0};
	for (size_t i {0}; i < str.size(); ++i)
	{
		if (str[i] == delim && len > 0)
		{
			out[index++] = sto<NumT>(trim_view(std::string_view(str.begin() + start, len)));
			len = 0;
			start = i + 1;
			continue;
		}
		++len;
	}

	if (len > 0)
		out[index++] = sto<NumT>(trim_view(std::string_view(str.begin() + start, len)));

	return out;
}

template <typename NumT, size_t Size, typename CharT>
std::array<NumT, Size> sto_array(std::basic_string<CharT> const & str, char const delim = ',')
{
	return sto_array<NumT, Size>(std::basic_string_view<CharT>(str));
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
/*
template <typename ContainerT, typename CharT>
ContainerT sto_container(std::basic_string<CharT> const & str, char const delim = ',')
{
 return sto_container(std::basic_string_view<CharT>(str), delim);
}
*/

/**
 * @brief Creates a container with a range of specified values with the following formats:
 *
 * [count] - range from 0 to count with an interval of 1
 * [start:count] - range from start to count with an interval of 1
 * [start:count:interval] - range from start to count with the specified interval
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
ContainerT sto_range(std::basic_string<CharT> const & str, char const delim = ':')
{
	if (str[0] != '[' || str[str.size() - 1] != ']')
		throw std::runtime_error("invalid range specification");
	std::string work_str {str.begin() + 1, str.end() - 1};
	auto vals = sto_container<std::vector<int>>(work_str, delim);
	int start, count, step;
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

	ContainerT out(count, 0);
	for (int iter {0}, val {start}; iter < count; ++iter, val += step)
		out[iter] = val;

	return out;
}

/**
 * @brief Parse a string as a boolean
 * @note Allowed values: "t", "true", "1" for true, "f", "false", "0" for false (upper or lower
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

/**
 * @brief Split a string on the specified char delimiter
 * @note Assumes input string has already been whitespace trimmed
 */
template <typename CharT>
std::pair<std::basic_string<CharT>, std::basic_string<CharT>> kvsplit(
	std::basic_string<CharT> const & line, std::string const & delim = " ")
{
	auto delim_pos {line.find_first_of(delim)};
	if (delim_pos == std::string::npos)
	{
		std::stringstream ss;
		ss << "Delimiter '" << delim << "' not found in string \"" << line << "\"";
		throw std::invalid_argument(ss.str());
	}
	return {line.substr(0, delim_pos), line.substr(delim_pos + 1, std::string::npos)};
}

template <typename CharT>
class basic_wsstring : std::basic_string<CharT>
{
public:
	using std::basic_string<CharT>::operator[];

	basic_wsstring<CharT>(CharT const * c) :
			std::basic_string<CharT>(c) {};

	bool operator==(std::string const & other) const
	{
		size_t other_index {0}, this_index {0};
		while (other[other_index] == ' ' || other[other_index] == '\t')
			++other_index;

		while (this_index < this->size())
			if ((*this)[this_index++] != other[other_index++])
				return false;

		if (other_index == other.size())
			return true;
		while (other[other_index] == ' ' || other[other_index] == '\t')
			++other_index;
		if (other_index == other.size())
			return true;

		return false;
	}
};

using wsstring = basic_wsstring<char>;

} // namespace motoi

#endif
