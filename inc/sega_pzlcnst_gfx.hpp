#ifndef SEGA_PZLCNST_GFX_H
#define SEGA_PZLCNST_GFX_H

#include "sega_md_gfx.hpp"

using namespace png;

namespace chrgfx
{
class sega_pzlcnst_px : public sega_md_px
{
 private:
	static const pal_traits traits;
	static const color* get_rgb_pzl(const u8* data);

 public:
	const color* get_rgb(const u8* data);
	const pal_traits* get_traits();
};

}	// namespace chrgfx
#endif