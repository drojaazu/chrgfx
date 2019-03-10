#ifndef NINTENDO_FC_GFX
#define NINTENDO_FC_GFX

#include "utils.hpp"

using namespace png;

namespace chrgfx
{
class nintendo_fc_cx : public chr_xform
{
 private:
	static const chr_traits traits;
	static const chr* get_chr_nes(u8* data);

 public:
	const chr_traits* get_traits();
	const chr* get_chr(u8* data);
};

class nintendo_fc_px : public pal_xform
{
 private:
	static const pal_traits traits;
	static const palette std_fc_pal;

 public:
	const pal_traits* get_traits();
	const color* get_rgb(u8* data);
	const palette* get_pal(u8* data);
	const palette* get_pal();
};

}	// namespace chrgfx

#endif