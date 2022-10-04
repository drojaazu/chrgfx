/**
 * @author Damian R (damian@motoi.pro)
 * @brief Convert encoded tile based graphics to indexed PNG
 * @version 1.3.0
 * 
 * @copyright ©2017 Motoi Productions / Released under MIT License
 *
 */

#ifndef __MOTOI__APP_HPP
#define __MOTOI__APP_HPP

/*
	These values should be set within CMakeLists.txt
*/
namespace APP
{
static unsigned int const VERSION_MAJOR {1};
static unsigned int const VERSION_MINOR {3};
static unsigned int const VERSION_PATCH {0};
static char const * VERSION {"1.3.0"};

static wchar_t const * NAME {L"chr2png"};
static wchar_t const * CONTACT {L"Damian R (damian@motoi.pro)"};
static wchar_t const * WEBSITE {L"https://github.com/drojaazu"};
static wchar_t const * BRIEF {L"Convert encoded tile based graphics to indexed PNG"};
} // namespace APP
#endif
