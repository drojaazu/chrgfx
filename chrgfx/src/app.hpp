#ifndef __MOTOI__APP_HPP
#define __MOTOI__APP_HPP

/*
	These values should be set within CMakeLists.txt
*/
namespace APP
{
static unsigned int const VERSION_MAJOR { 2 };
static unsigned int const VERSION_MINOR { 1 };
static unsigned int const VERSION_PATCH { 0 };
static char const * VERSION { "2.1.0" };

static wchar_t const * NAME { L"chrgfx" };
static wchar_t const * CONTACT { L"Damian R (damian@motoi.pro)" };
static wchar_t const * WEBSITE { L"https://github.com/drojaazu" };
} // namespace APP
#endif
