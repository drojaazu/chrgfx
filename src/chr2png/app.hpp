/**
 * @author Damian R (damian@motoi.pro)
 * @brief Convert encoded tile graphics to PNG image
 * @version 1.3.0
 * 
 * @copyright ©2017 Motoi Productions / Released under MIT License
 *
 */

#ifndef __MOTOI__APP_HPP
#define __MOTOI__APP_HPP

#include <string>
#include <sstream>

/*
	These values should be set within CMakeLists.txt
*/
namespace APP
{
static unsigned int const VERSION_MAJOR {1};
static unsigned int const VERSION_MINOR {3};
static unsigned int const VERSION_PATCH {0};
static char const * VERSION {"1.3.0"};

static char const * NAME {"chr2png"};
static char const * CONTACT {"Damian R (damian@motoi.pro)"};
static char const * WEBSITE {"https://github.com/drojaazu"};
static char const * BRIEF {"Convert encoded tile graphics to PNG image"};

std::string app_version()
{
	std::stringstream ss;
	ss << APP::NAME << " - version " << APP::VERSION << '\n';
	ss << APP::CONTACT << " / " << APP::WEBSITE << '\n';

	return ss.str();
}

} // namespace APP
#endif
