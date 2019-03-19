#ifndef SEGA_MD_GFX_H
#define SEGA_MD_GFX_H

#include "utils.hpp"

using namespace png;

namespace chrgfx
{
class sega_md_cx : public chr_xform
{
 private:
	static const chr_traits traits;
	static const chr* get_chr_smd(const u8* data);

 public:
	const chr_traits* get_traits();
	const chr* get_chr(const u8* data);
};

class sega_md_px : public pal_xform
{
 private:
	static const pal_traits traits;
	static const color* get_rgb_smd(const u8* data);

 public:
	const pal_traits* get_traits();
	const color* get_rgb(const u8* data);
	const palette* get_pal(const u8* data, s16 subpal);
};
}	// namespace chrgfx

#endif