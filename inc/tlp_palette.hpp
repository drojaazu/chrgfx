#ifndef TLP_PALETTE_H
#define TLP_PALETTE_H

#include "utils.hpp"

using namespace png;

namespace chrgfx
{
class tilelayerpro_px : public pal_xform
{
 private:
	static const palette* get_pal_tlp(const u8* data);
	static const pal_traits traits;

 public:
	const pal_traits* get_traits();
	const color* get_rgb(const u8* data);
	const palette* get_pal(const u8* data, int8_t subpal);
};

}	// namespace chrgfx

#endif