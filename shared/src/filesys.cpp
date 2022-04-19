/**
 * @file filesys.cpp
 * @author Motoi Productions (Damian Rogers damian@motoi.pro)
 * @brief File system/path utilities
 *
 * Updates:
 * 20211214 Initial
 */

#include "filesys.hpp"
#include <cstring>
#include <dirent.h>
#include <sstream>
#include <stdexcept>
#include <unistd.h>

using namespace std;

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

std::ifstream ifstream_checked(char const * path, char const * purpose)
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

std::ofstream ofstream_checked(char const * path, char const * purpose)
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
