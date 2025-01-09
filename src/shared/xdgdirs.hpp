/**
 * @file xdgdirs.hpp
 * @author Damian Rogers / damian@motoi.pro
 * @brief Get files from system paths using XDG
 * @copyright ©2024 Motoi Productions / Released under MIT License
 *
 * Updates:
 * 20240106 Initial
 */

#ifndef __MOTOI__XDGDIRS_HPP
#define __MOTOI__XDGDIRS_HPP

#include <string>
#include <sys/stat.h>
#include <vector>

#include "filesys.hpp"

// https://specifications.freedesktop.org/basedir-spec/basedir-spec-latest.html

/*
The order of base directories denotes their importance; the first directory listed is the most important. When the same
information is defined in multiple places the information defined relative to the more important base directory takes
precedent. The base directory defined by $XDG_DATA_HOME is considered more important than any of the base directories
defined by $XDG_DATA_DIRS. The base directory defined by $XDG_CONFIG_HOME is considered more important than any of the
base directories defined by $XDG_CONFIG_DIRS.
*/

std::string xdg_data_dirs();

std::string xdg_data_home();

std::string xdg_config_dirs();

std::string xdg_config_home();

/**
 * @brief Returns a list of full filepaths for a given requested filename, sorted in ascending order of precedence
 *
 * @param filename
 * @return std::vector<std::string>
 */
template <typename CharT>
std::vector<std::basic_string<CharT>> data_filepaths(std::basic_string<CharT> const & filename)
{
	std::vector<std::basic_string<CharT>> filepaths;

	std::basic_string<CharT> test_path;
	// check all XDG_DATA_DIRS entries next
	std::basic_string<CharT> data_dirs {xdg_data_dirs()};
	int pos = 0;
	while ((pos = data_dirs.find(':')) != std::string::npos)
	{
		test_path = concat_paths(data_dirs.substr(0, pos), filename);
		if (file_exists(test_path))
			filepaths.insert(filepaths.begin(), test_path);
		data_dirs.erase(0, pos + 1);
	}
	// account for the remaining entry after the last found colon
	test_path = concat_paths(data_dirs.substr(0, pos), filename);
	if (file_exists(test_path))
		filepaths.insert(filepaths.begin(), test_path);

	// check XDG_DATA_HOME last as it is most important
	test_path = concat_paths(xdg_data_home(), filename);
	if (file_exists(test_path))
		filepaths.push_back(test_path);

	return filepaths;
}

std::vector<std::string> data_filepaths(char const * filename);

/**
 * @brief Returns a list of full filepaths for a given requested filename, sorted in order of precedence
 *
 * @param filename
 * @return std::vector<std::string>
 */
template <typename CharT>
std::vector<std::basic_string<CharT>> config_filepaths(std::basic_string<CharT> const & filename)
{
	std::vector<std::basic_string<CharT>> filepaths;
	std::basic_string<CharT> test_path = concat_paths(xdg_config_home(), filename);
	if (file_exists(test_path))
		filepaths.push_back(test_path);

	std::basic_string<CharT> config_dirs {xdg_config_dirs()};
	int pos = 0;
	while ((pos = config_dirs.find(':')) != std::string::npos)
	{
		test_path = concat_paths(config_dirs.substr(0, pos), filename);
		if (file_exists(test_path))
			filepaths.push_back(test_path);
		config_dirs.erase(0, pos + 1);
	}
	test_path = concat_paths(config_dirs.substr(0, pos), filename);
	if (file_exists(test_path))
		filepaths.push_back(test_path);

	return filepaths;
}

std::vector<std::string> config_filepaths(char const * filename);

#endif
