#include "capcom_cps_gfx.hpp"

using namespace png;

namespace chrgfx
{
// ----------------- CHR
const chr_traits capcom_cps_cx::traits = {
		16,	// 16px width
		16,	// 16px height
		4,	 // 4bpp
		128	// 128 bytes
};

const u16 CHR_PXL_COUNT = 256;	// traits.width * traits.height

const chr_traits* capcom_cps_cx::get_traits() { return &capcom_cps_cx::traits; }

const chr* capcom_cps_cx::get_chr(u8* data) { return get_chr_cps(data); }

const chr* capcom_cps_cx::get_chr_cps(u8* data)
{
	u8 this_pxl, pxl_idx{0};

	chr* _out = new chr[CHR_PXL_COUNT];

	for(u16 row = 0; row < 128; row += 4)
	{
		for(int8_t shift = 7; shift >= 0; shift--)
		{
			this_pxl = 0;
			if((data[row] >> shift) & 1) this_pxl |= 1;
			if((data[row + 1] >> shift) & 1) this_pxl |= 2;
			if((data[row + 2] >> shift) & 1) this_pxl |= 4;
			if((data[row + 3] >> shift) & 1) this_pxl |= 8;
			_out[pxl_idx++] = this_pxl;
		}
	}

	return _out;
}

}	// namespace chrgfx