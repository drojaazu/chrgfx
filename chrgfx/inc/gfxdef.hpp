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
public:
	gfxdef () = delete;
	gfxdef (gfxdef &&) = default;
	gfxdef (gfxdef const &) = default;
	gfxdef & operator= (gfxdef const &) = delete;
	gfxdef & operator= (gfxdef &&) = delete;

	[[nodiscard]] std::string const & id () const;

	[[nodiscard]] std::string const & description () const;

protected:
	gfxdef (std::string id, std::string description = "");

	std::string const m_id;
	std::string const m_desc;
};

} // namespace chrgfx

#endif
