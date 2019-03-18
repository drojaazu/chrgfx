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
	static const chr* get_chr_sfc(const u8* data);

 public:
	const chr_traits* get_traits();
	const chr* get_chr(const u8* data);
};

class nintendo_sfc_3bpp_cx : public chr_xform
{
 private:
	static const chr_traits traits;
	static const chr* get_chr_sfc_3bpp(const u8* data);

 public:
	const chr_traits* get_traits();
	const chr* get_chr(const u8* data);
};

class nintendo_sfc_8bpp_cx : public chr_xform
{
 private:
	static const chr_traits traits;
	static const chr* get_chr_sfc_8bpp(const u8* data);

 public:
	const chr_traits* get_traits();
	const chr* get_chr(const u8* data);
};

class nintendo_sfc_px : public pal_xform
{
 private:
	static const pal_traits traits;
	static const color* get_rgb_sfc(const u8* data);

 public:
	const pal_traits* get_traits();
	const color* get_rgb(const u8* data);
	const palette* get_pal(const u8* data, int8_t subpal);
};

}	// namespace chrgfx

#endif