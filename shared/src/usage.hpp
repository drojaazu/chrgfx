/**
 * @file usage.hpp
 * @author Motoi Productions (Damian Rogers damian@motoi.pro)
 * @brief Display program options/usage
 *
 * Updates:
 * 20220415 Initial
 * 20220722 Using const in show_usage parameters
 * 20220914 Added show_version
 */

#ifndef __MOTOI__USAGE_HPP
#define __MOTOI__USAGE_HPP

#include <getopt.h>
#include <iostream>
#include <ostream>
#include <sstream>

struct option_details
{
	bool const required;
	char const * const desc;
	char const * const arg_type;
};

std::string show_usage(option const * opts, option_details const * details);

#endif
