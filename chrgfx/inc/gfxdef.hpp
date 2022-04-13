#ifndef __CHRGFX__GFXDEF_HPP
#define __CHRGFX__GFXDEF_HPP

#include <string>

namespace chrgfx
{
/**
 * @brief Abstract encoding class
 *
 */
class gfxdef
{
public:
	gfxdef() = delete;
	gfxdef(gfxdef &&) = default;
	gfxdef(gfxdef const &) = default;
	gfxdef & operator=(gfxdef const &) = delete;
	gfxdef & operator=(gfxdef &&) = delete;

	std::string const & id() const;

protected:
	gfxdef(std::string const & id);

	std::string const m_id;
};

} // namespace chrgfx

#endif
