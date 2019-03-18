#ifndef NINTENDO_GB_GFX
#define NINTENDO_GB_GFX

#include "utils.hpp"

using namespace png;

namespace chrgfx
{
class nintendo_gb_cx : public chr_xform
{
 private:
	static const chr_traits traits;
	static const chr* get_chr_gb(const u8* data);

 public:
	const chr_traits* get_traits();
	const chr* get_chr(const u8* data);
};

class nintendo_gb_px : public pal_xform
{
 private:
	static const pal_traits traits;

 protected:
	static const palette* get_gb_pal(const u8* data, const color* gb,
																	 int8_t subpal);
	static const color gameboy_colors_original[];
	static const color gameboy_colors_pocket[];

 public:
	const pal_traits* get_traits();
	const color* get_rgb(const u8* data);
	const palette* get_pal(const u8* data, int8_t subpal);
};

class nintendo_gbpocket_px : public nintendo_gb_px
{
 public:
	const color* get_rgb(const u8* data);
	const palette* get_pal(const u8* data, int8_t subpal);
};

class nintendo_gbcolor_px : public pal_xform
{
 private:
	static const pal_traits traits;

 public:
	const pal_traits* get_traits();
	const color* get_rgb(const u8* data);
	const palette* get_pal(const u8* data, int8_t subpal);
};

}	// namespace chrgfx

#endif