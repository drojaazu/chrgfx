#ifndef TLP_PALETTE_H
#define TLP_PALETTE_H

#include "utils.hpp"

using namespace png;

namespace chrgfx
{
class tilelayerpro_px : public pal_xform
{
 private:
	static const palette* get_pal_tlp(u8* data);
	static const pal_traits traits;

 public:
	const pal_traits* get_traits();
	const color* get_rgb(u8* data);
	const palette* get_pal(u8* data);
};

}	// namespace chrgfx

#endif