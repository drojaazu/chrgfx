#ifndef SNK_NGP_GFX
#define SNK_NGP_GFX

#include "utils.hpp"

using namespace png;

// palette ram: 312 bytes

namespace chrgfx
{
class snk_ngp_cx : public chr_xform
{
 private:
	static const chr_traits traits;
	static const chr* get_chr_ngp(const u8* data);

 public:
	const chr_traits* get_traits();
	const chr* get_chr(const u8* data);
};

class snk_ngp_px : public pal_xform
{
 private:
	static const pal_traits traits;
	static const color ngp_colors[];

 public:
	const pal_traits* get_traits();
	const color* get_rgb(const u8* data);
	const palette* get_pal(const u8* data, int8_t subpal);
};

class snk_ngpc_px : public pal_xform
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