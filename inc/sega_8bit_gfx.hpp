#ifndef SEGA_8BIT_GFX
#define SEGA_8BIT_GFX

#include "utils.hpp"

namespace chrgfx
{
class sega_8bit_cx : public chr_xform
{
 private:
	static const chr_traits traits;
	static const chr* get_chr_sega8bit(u8* data);

 public:
	const chr_traits* get_traits();
	const chr* get_chr(u8* data);
};

class sega_mastersys_px : public pal_xform
{
 private:
	static const pal_traits traits;

 public:
	const pal_traits* get_traits();
	const color* get_rgb(u8* data);
	const palette* get_pal(u8* data);
};

class sega_gamegear_px : public pal_xform
{
 private:
	static const pal_traits traits;

 public:
	const pal_traits* get_traits();
	const color* get_rgb(u8* data);
	const palette* get_pal(u8* data);
};

}	// namespace chrgfx
#endif
