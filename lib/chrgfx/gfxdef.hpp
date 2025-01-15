/**
 * @file gfxdef.hpp
 * @author Damian Rogers / damian@motoi.pro
 * @copyright ©2022 Motoi Productions / Released under MIT License
 * @brief Graphics format definition base class
 */

#ifndef __CHRGFX__GFXDEF_HPP
#define __CHRGFX__GFXDEF_HPP

#include <string>

namespace chrgfx
{
/**
 * @brief Abstract encoding class
 */
class gfxdef
{
protected:
	gfxdef(std::string id, std::string description = "");

	std::string m_id;
	std::string m_desc;

public:
	gfxdef(gfxdef &&) = default;
	gfxdef(gfxdef const &) = default;
	gfxdef & operator=(gfxdef const &) = default;
	gfxdef & operator=(gfxdef &&) = default;

	[[nodiscard]] std::string const & id() const;

	[[nodiscard]] std::string const & note() const;
};

} // namespace chrgfx

#endif
