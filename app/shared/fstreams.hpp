/**
 * @file fstreams.hpp
 * @author Damian Rogers (damian@motoi.pro)
 * @brief Checked file streams
 * @copyright Motoi Productions / Released under MIT License
 *
 * Updates:
 * 20220916 Initial
 * 20221118 Changed exception to std::system_error
 * 20230415 Added default string type; removed extraneous errno message
 */

#ifndef __MOTOI__FSTREAMS_HPP
#define __MOTOI__FSTREAMS_HPP

#include <fstream>
#include <sstream>
#include <system_error>

namespace motoi
{

template <typename StringT = char>
std::ifstream ifstream_checked(
	std::basic_string<StringT> const & path, std::basic_string<StringT> const & purpose = "read")
{
	std::ifstream ifs(path);
	if (! ifs.good())
	{
		std::basic_ostringstream<StringT> oss;
		oss << "Could not open \"" << path << "\" for " << purpose;
		throw std::system_error(errno, std::system_category(), oss.str());
	}
	return ifs;
}

template <typename StringT = char>
std::ofstream ofstream_checked(
	std::basic_string<StringT> const & path, std::basic_string<StringT> const & purpose = "write")
{
	std::ofstream ofs(path);
	if (! ofs.good())
	{
		std::basic_ostringstream<StringT> oss;
		oss << "Could not open \"" << path << "\" for " << purpose;
		throw std::system_error(errno, std::system_category(), oss.str());
	}
	return ofs;
}
} // namespace motoi

#endif
