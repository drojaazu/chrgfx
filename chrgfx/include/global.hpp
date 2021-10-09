#ifndef CHRGFX__GLOBAL_H
#define CHRGFX__GLOBAL_H

#include <string>

// library globals
unsigned int const LIB_VERSION_MAJOR{1};
unsigned int const LIB_VERSION_MINOR{0};
unsigned int const LIB_VERSION_FIX{0};
std::string const LIB_VERSION{std::to_string(LIB_VERSION_MAJOR) + "." +
															std::to_string(LIB_VERSION_MINOR) + "." +
															std::to_string(LIB_VERSION_FIX)};
std::string const LIB_NAME{"libchrgfx"};
std::string const LIB_CONTACT{"Damian R (damian@sudden-desu.net)"};
std::string const LIB_WEBSITE{"https://github.com/drojaazu/chrgfx"};

#endif