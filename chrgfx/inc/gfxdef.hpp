#ifndef __CHRGFX__GFXDEF_HPP
#define __CHRGFX__GFXDEF_HPP

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

	char const * id() const;

protected:
	gfxdef(char const * id);

	char const * m_id;
};

} // namespace chrgfx

#endif
