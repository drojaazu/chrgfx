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

#include <fstream>
#include <sys/stat.h>

struct stat stat(char const * path);

bool exists(char const * path);

std::ifstream ifstream_checked(char const * path,
															 char const * purpose = "read");

std::ofstream ofstream_checked(char const * path,
															 char const * purpose = "write");

#endif
