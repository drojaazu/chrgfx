#ifndef NINTENDO_SFC_GFX
#define NINTENDO_SFC_GFX

#include "utils.hpp"

using namespace png;

namespace chrgfx
{
class nintendo_sfc_cx : public chr_xform
{
 private:
	static const chr_traits traits;
	static const chr* get_chr_sfc(u8* data);

 public:
	const chr_traits* get_traits();
	const chr* get_chr(u8* data);
};

class nintendo_sfc_3bpp_cx : public chr_xform
{
 private:
	static const chr_traits traits;
	static chr* get_chr_sfc_3bpp(u8* data);

 public:
	const chr_traits* get_traits();
	const chr* get_chr(u8* data);
};

class nintendo_sfc_px : public pal_xform
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