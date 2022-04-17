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
#include <utility>
#include <vector>

/**
 * @brief Removes any spaces or tabs on the left side of a string
 *
 * @param s Mutable string
 */
template <typename T> void ltrim(std::basic_string<T> & s);

/**
 * @brief Removes any spaces or tabs on the right side of a string
 *
 * @param s Mutable string
 */
template <typename T> void rtrim(std::basic_string<T> & s);

/**
 * @brief Removes any spaces or tabs on the left and right side of a string
 *
 * @param s Mutable string
 */
template <typename T> void trim(std::basic_string<T> & s);

/**
 * @brief Converts a string with alphabetic characters to lower case
 *
 * @param s Mutable string
 */
template <typename T> void lower(std::basic_string<T> & s);

/**
 * @brief Converts a string with alphabetic characters to upper case
 *
 * @param s Mutable string
 */
template <typename T> void upper(std::basic_string<T> & s);

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
template <typename T = unsigned short>
T sto(std::string const & s, int const base = 10);

template <typename T = unsigned short>
T sto_pos(std::string const & s, int const base = 10);

/**
 * @brief Converts a delimited list of integer values into a vector
 */
template <typename T = unsigned short>
std::vector<T> stovec(std::string const & s, char const delim = ',');

/**
 * @brief Parse a string as a boolean
 * @note Allowed values: "t", "true", "1", "f", "false", "0" (upper or lower
 * case)
 */
bool stob(std::string const & s);

/**
 * @brief Split a string on the specified char delimiter
 * @note Assumes input string has already been whitespace trimmed
 */
std::pair<std::string, std::string> kvsplit(std::string const & line,
																						char const delim = ' ');

#endif