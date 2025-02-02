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
 * 20240109 Consolidated checked filestream methods into this header; fixes to thrown exception types
 */

#ifndef __MOTOI__FILESYS_HPP
#define __MOTOI__FILESYS_HPP

#include <cstring>
#include <fstream>
#include <sstream>
#include <sys/stat.h>
#include <unistd.h>

namespace motoi
{
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
	for (auto const & filepath : {paths...})
	{
		++u;
		if (filepath.empty())
			continue;
		if (filepath.front() == '/')
			oss.seekp(-1, std::ios::cur);
		oss << filepath;
		if (filepath.back() != '/' && u < path_count)
			oss << '/';
	}

	return oss.str();
}

template <typename CharT>
struct stat file_stat(std::basic_string<CharT> const & filepath)
{
	static struct stat status;
	if (::stat(filepath.c_str(), &status) != 0)
	{
		std::basic_ostringstream<CharT> oss;
		oss << "Could not open \"" << filepath << "\": " << strerror(errno);
		throw std::system_error(oss.str());
	}
	return status;
}

template <typename CharT>
inline bool file_exists(std::basic_string<CharT> const & filepath)
{
	static struct stat status;
	return (::stat(filepath.c_str(), &status) == 0);
}

template <typename CharT>
inline size_t file_size(std::basic_string<CharT> const & filepath)
{
	return file_stat(filepath).st_size;
}

template <typename CharT>
std::basic_string<CharT> strip_extension(std::basic_string<CharT> const & filepath)
{
	auto pos {filepath.find_last_of('.')};
	if (pos == std::string::npos)
		return filepath;
	return filepath.substr(0, pos);
}

template <typename CharT>
std::basic_string<CharT> path(std::basic_string<CharT> const & filepath)
{
	auto pos {filepath.find_last_of('/')};
	if (pos == std::string::npos)
		return filepath;
	return filepath.substr(0, pos);
}

template <typename CharT>
std::basic_string<CharT> filename(std::basic_string<CharT> const & filepath)
{
	auto pos {filepath.find_last_of('/')};
	if (pos == std::string::npos)
		return filepath;
	return filepath.substr(pos + 1, filepath.size() - pos);
}

template <typename CharT>
std::ifstream ifstream_checked(
	std::basic_string<CharT> const & filepath, std::basic_string<CharT> const & purpose = "read")
{
	std::ifstream ifs(filepath);
	if (! ifs.good())
	{
		std::basic_ostringstream<CharT> oss;
		oss << "Could not open \"" << filepath << "\" for " << purpose;
		throw std::system_error(errno, std::generic_category(), oss.str());
	}
	return ifs;
}

template <typename CharT>
std::ofstream ofstream_checked(
	std::basic_string<CharT> const & filepath, std::basic_string<CharT> const & purpose = "write")
{
	std::ofstream ofs(filepath);
	if (! ofs.good())
	{
		std::basic_ostringstream<CharT> oss;
		oss << "Could not open \"" << filepath << "\" for " << purpose;
		throw std::system_error(errno, std::generic_category(), oss.str());
	}
	return ofs;
}

template <typename CharT>
std::fstream fstream_checked(
	std::basic_string<CharT> const & filepath, std::basic_string<CharT> const & purpose = "read/write")
{
	std::fstream fs(filepath);
	if (! fs.good())
	{
		std::basic_ostringstream<CharT> oss;
		oss << "Could not open \"" << filepath << "\" for " << purpose;
		throw std::system_error(errno, std::generic_category(), oss.str());
	}
	return fs;
}
} // namespace motoi

#endif
