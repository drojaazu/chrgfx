/**
 * @file fstreams.hpp
 * @author Motoi Productions (Damian Rogers damian@motoi.pro)
 * @brief Checked file streams
 *
 * Updates:
 * 20220916 Initial
 */

#ifndef __MOTOI__FSTREAMS_HPP
#define __MOTOI__FSTREAMS_HPP

#include <cstring>
#include <fstream>
#include <sstream>
#include <stdexcept>

template <typename StringT>
std::ifstream ifstream_checked(std::basic_string<StringT> const & path,
	std::basic_string<StringT> const & purpose = "read")
{
	std::ifstream ifs(path);
	if(! ifs.good())
	{
		std::basic_ostringstream<StringT> oss;
		oss << "Could not open input path \"" << path << "\" for " << purpose
				<< ": " << strerror(errno);
		throw std::runtime_error(oss.str());
	}
	return ifs;
}

template <typename StringT>
std::ofstream ofstream_checked(std::basic_string<StringT> const & path,
	std::basic_string<StringT> const & purpose = "write")
{
	std::ofstream ofs(path);
	if(! ofs.good())
	{
		std::basic_ostringstream<StringT> oss;
		oss << "Could not open output path \"" << path << "\" for " << purpose
				<< ": " << strerror(errno);
		throw std::runtime_error(oss.str());
	}
	return ofs;
}

#endif
