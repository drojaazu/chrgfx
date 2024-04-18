/**
 * @file filesys.hpp
 * @author Damian Rogers / damian@motoi.pro
 * @brief File system/path utilities
 * @copyright ©2021 Motoi Productions / Released under MIT License
 *
 * Updates:
 * 20211214 Initial
 * 20220420 Converted to basic_string<type>, added path parsing functions
 * 20220720 Change stringstream to ostringstream; added file_size & concat_paths
 */

#ifndef __MOTOI__FILESYS_HPP
#define __MOTOI__FILESYS_HPP

#include <cstring>
#include <sstream>
#include <sys/stat.h>

template <typename StringT,
	typename... StringTs,
	typename = std::enable_if_t<std::conjunction_v<std::is_same<StringT, StringTs>...>>>
std::basic_string<typename StringT::value_type> concat_paths(StringT const & base_path, StringTs const &... paths)
{
	// enable_if + conjunction_v + is_same ensures all variadic parameters are the same type
	std::basic_ostringstream<typename StringT::value_type> oss;
	oss << base_path;
	if (base_path.back() != '/')
		oss << '/';
	
	size_t path_count = sizeof...(paths);
	size_t u = 0;
	for (auto const & path : {paths...})
	{
		++u;
		if (path.empty())
			continue;
		if (path.front() == '/')
			oss.seekp(-1, std::ios::cur);
		oss << path;
		if (path.back() != '/' && u < path_count)
			oss << '/';
	}

	return oss.str();
}

template <typename CharT>
struct stat file_stat(std::basic_string<CharT> const & path)
{
	static struct stat status;
	if (::stat(path.c_str(), &status) != 0)
	{
		std::basic_ostringstream<CharT> oss;
		oss << "Could not open path " << path << ": " << strerror(errno);
		throw runtime_error(oss.str());
	}
	return status;
}

template <typename CharT>
inline bool file_exists(std::basic_string<CharT> const & path)
{
	static struct stat status;
	return (::stat(path.c_str(), &status) == 0);
}

template <typename CharT>
inline size_t file_size(std::basic_string<CharT> const & path)
{
	return file_stat(path).st_size;
}

template <typename CharT>
std::basic_string<CharT> strip_extension(std::basic_string<CharT> const & path)
{
	auto pos {path.find_last_of('.')};
	if (pos == std::string::npos)
		return path;
	return path.substr(0, pos);
}

template <typename CharT>
std::basic_string<CharT> filename_from_path(std::basic_string<CharT> const & path)
{
	auto pos {path.find_last_of('/')};
	if (pos == std::string::npos)
		return path;
	return path.substr(0, pos);
}

#endif
