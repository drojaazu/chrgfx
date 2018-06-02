#ifndef BPP1_GFX_H
#define BPP1_GFX_H

#include "utils.hpp"

using namespace png;

namespace gfx
{
class bpp1_cx : public chr_xform
{
 private:
	static const chr_traits traits;
	static const chr* get_chr_1bpp(BYTE* data);

 public:
	const chr_traits* get_traits();
	const chr* get_chr(BYTE* data);
};

}	// namespace gfx

#endif