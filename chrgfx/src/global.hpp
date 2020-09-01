#ifndef CHRGFX__GLOBAL_H
#define CHRGFX__GLOBAL_H

#include <string>

// library globals
static unsigned int const LIB_VERSION_MAJOR{1};
static unsigned int const LIB_VERSION_MINOR{0};
static unsigned int const LIB_VERSION_FIX{0};
static std::string const LIB_VERSION{std::to_string(LIB_VERSION_MAJOR) + "." +
																		 std::to_string(LIB_VERSION_MINOR) + "." +
																		 std::to_string(LIB_VERSION_FIX)};
static std::string const LIB_NAME{"libchrgfx"};

#endif