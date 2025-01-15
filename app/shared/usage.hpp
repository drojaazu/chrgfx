/**
 * @file usage.hpp
 * @author Motoi Productions (Damian Rogers damian@motoi.pro)
 * @brief Display program options/usage
 * @copyright ©2024 Motoi Productions / Released under MIT License
 *
 * Updates:
 * 20220415 Initial
 * 20220722 Using const in show_usage parameters
 * 20220914 Added show_version
 * 20221118 Added copyright field; change to non-wide characters to properly support
 * 	UTF-8; updates to match changes to match app.hpp.cfg
 */

#ifndef __MOTOI__USAGE_HPP
#define __MOTOI__USAGE_HPP

#include <getopt.h>
#include <iostream>
#include <ostream>

namespace motoi
{
struct option_details
{
	bool const required;
	char const * const desc;
	char const * const arg_type;
};

/**
 * @brief Displays program version
 *
 * @param output stream to write the version
 */
void show_version(std::ostream & output);

/**
 * @brief Displays program options and usage
 *
 * @param opts array of @c option structs; final entry should be all zero
 * @param details array of @c option_details structs; must be the same size and
 * order as @c opts
 * @param output stream to write the usage
 */
void show_usage(option const * opts, option_details const * details, std::ostream & output = std::cout);

} // namespace motoi

#endif
