/**
 * @file filesys.hpp
 * @author Motoi Productions (Damian Rogers damian@motoi.pro)
 * @brief File system/path utilities
 *
 * Updates:
 * 20211214 Initial
 */

#ifndef __MOTOI__FILESYS_HPP
#define __MOTOI__FILESYS_HPP

#include <cstring>
#include <dirent.h>
#include <fstream>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <sys/stat.h>
#include <unistd.h>

using std::runtime_error;
using std::string;
using std::stringstream;

struct stat stat(char const * path)
{
	static struct stat status;
	if(::stat(path, &status) != 0)
	{
		stringstream ss;
		ss << "Could not open path " << path << ": " << strerror(errno);
		throw runtime_error(ss.str());
	}
	return status;
}

bool exists(char const * path)
{
	static struct stat status;
	return (::stat(path, &status) == 0);
}

std::ifstream ifstream_checked(char const * path, char const * purpose = "read")
{
	std::ifstream ifs(path);
	if(!ifs.good())
	{
		stringstream ss;
		ss << "Could not open input path \"" << path << "\" for " << purpose << ": "
			 << strerror(errno);
		throw runtime_error(ss.str());
	}
	return ifs;
}

std::ofstream ofstream_checked(char const * path,
															 char const * purpose = "write")
{
	std::ofstream ofs(path);
	if(!ofs.good())
	{
		stringstream ss;
		ss << "Could not open output path \"" << path << "\" for " << purpose
			 << ": " << strerror(errno);
		throw runtime_error(ss.str());
	}
	return ofs;
}

#endif
