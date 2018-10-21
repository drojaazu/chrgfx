#ifndef CAPCOM_CPS1_GFX
#define CAPCOM_CPS1_GFX

#include "utils.hpp"

using namespace png;

namespace gfx
{
class capcom_cps_cx : public chr_xform
{
 private:
	static const chr_traits traits;
	static const chr* get_chr_cps(BYTE* data);

 public:
	const chr_traits* get_traits();
	const chr* get_chr(BYTE* data);
};

}	// namespace gfx

#endif