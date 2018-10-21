#include "capcom_cps_gfx.hpp"

using namespace png;

namespace gfx
{
// -------- CHR
const chr_traits capcom_cps_cx::traits = {
		16,	// 16px width
		16,	// 16px height
		4,	 // 4bpp
		128	// 128 bytes
};

const chr_traits* capcom_cps_cx::get_traits() { return &capcom_cps_cx::traits; }

const chr* capcom_cps_cx::get_chr(BYTE* data) { return get_chr_cps(data); }

const chr* capcom_cps_cx::get_chr_cps(BYTE* data)
{
	auto _out = new chr(traits.width, traits.height);
	size_t pxlRowCount = 0;
	BYTE thisPxl;
	auto thisPxlRow = std::vector<index_pixel>();

	// need to put some thought into making this code a bit more graceful
	// but it works for now
	for(size_t row = 0; row < 128; row += 4)
	{
		thisPxlRow.clear();
		thisPxlRow.reserve(traits.width);
		for(int8_t shift = 7; shift >= 0; shift--)
		{
			thisPxl = 0;
			if((data[row] >> shift) & 1) thisPxl++;
			if((data[row + 1] >> shift) & 1) thisPxl += 2;
			if((data[row + 2] >> shift) & 1) thisPxl += 4;
			if((data[row + 3] >> shift) & 1) thisPxl += 8;
			thisPxlRow.push_back(thisPxl);
		}
		row += 4;
		for(int8_t shift = 7; shift >= 0; shift--)
		{
			thisPxl = 0;
			if((data[row] >> shift) & 1) thisPxl++;
			if((data[row + 1] >> shift) & 1) thisPxl += 2;
			if((data[row + 2] >> shift) & 1) thisPxl += 4;
			if((data[row + 3] >> shift) & 1) thisPxl += 8;
			thisPxlRow.push_back(thisPxl);
		}
		_out->put_row(pxlRowCount, thisPxlRow);
		pxlRowCount++;
	}

	return _out;
}

}	// namespace gfx