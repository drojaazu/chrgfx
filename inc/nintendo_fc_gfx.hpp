#ifndef NINTENDO_FC_GFX
#define NINTENDO_FC_GFX

#include "utils.hpp"

using namespace png;

namespace gfx
{
class nintendo_fc_cx : public chr_xform
{
 private:
	static const chr_traits traits;
	static const chr* get_chr_nes(BYTE* data);

 public:
	const chr_traits* get_traits();
	const chr* get_chr(BYTE* data);
};

class nintendo_fc_px : public pal_xform
{
 private:
	static const pal_traits traits;
	static const palette std_fc_pal;

 public:
	const pal_traits* get_traits();
	const color* get_rgb(BYTE* data);
	const palette* get_pal(BYTE* data);
	const palette* get_pal();
};

}	// namespace gfx

#endif