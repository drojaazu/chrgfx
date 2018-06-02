#ifndef TLP_PALETTE_H
#define TLP_PALETTE_H

#include "utils.hpp"

using namespace png;

namespace gfx
{
class tilelayerpro_px : public pal_xform
{
 private:
	static const palette* get_pal_tlp(BYTE* data);
	static const pal_traits traits;

 public:
	const pal_traits* get_traits();
	const color* get_rgb(BYTE* data);
	const palette* get_pal(BYTE* data);
};

}	// namespace gfx

#endif