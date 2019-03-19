#include "snk_neogeo.hpp"

using namespace png;

namespace chrgfx
{
// ----------------- CHR
const chr_traits snk_neogeo_cx::traits = {
		16,	// 16px width
		16,	// 16px height
		4,	 // 4bpp
		128	// 128 bytes
};

const u16 CHR_PXL_COUNT = 256;

const chr_traits* snk_neogeo_cx::get_traits() { return &snk_neogeo_cx::traits; }

const chr* snk_neogeo_cx::get_chr(const u8* data)
{
	return get_chr_neogeo(data);
}

const chr* snk_neogeo_cx::get_chr_neogeo(const u8* data)
{
	auto _out = new chr[CHR_PXL_COUNT];
	u8 this_pxl, pxl_idx{0};
	// read chr 2 first, offset 64 bytes
	data += 64;
	for(u8 row = 0; row < 32; row += 4)
	{
		for(u8 shift = 0; shift < 8; shift++)
		{
			this_pxl = 0;
			if((data[row] >> shift) & 1) this_pxl |= 1;
			if((data[row + 1] >> shift) & 1) this_pxl |= 2;
			if((data[row + 2] >> shift) & 1) this_pxl |= 4;
			if((data[row + 3] >> shift) & 1) this_pxl |= 8;
			_out[pxl_idx++] = this_pxl;
		}
		pxl_idx += 8;
	}

	// read chr 0 next, offset 0
	data -= 64;
	pxl_idx = 8;
	for(u8 row = 0; row < 32; row += 4)
	{
		for(u8 shift = 0; shift < 8; shift++)
		{
			this_pxl = 0;
			if((data[row] >> shift) & 1) this_pxl |= 1;
			if((data[row + 1] >> shift) & 1) this_pxl |= 2;
			if((data[row + 2] >> shift) & 1) this_pxl |= 4;
			if((data[row + 3] >> shift) & 1) this_pxl |= 8;
			_out[pxl_idx++] = this_pxl;
		}
		pxl_idx += 8;
	}

	// read chr 3 next, offset 96
	data += 96;
	pxl_idx = 128;
	for(u8 row = 0; row < 32; row += 4)
	{
		for(u8 shift = 0; shift < 8; shift++)
		{
			this_pxl = 0;
			if((data[row] >> shift) & 1) this_pxl |= 1;
			if((data[row + 1] >> shift) & 1) this_pxl |= 2;
			if((data[row + 2] >> shift) & 1) this_pxl |= 4;
			if((data[row + 3] >> shift) & 1) this_pxl |= 8;
			_out[pxl_idx++] = this_pxl;
		}
		pxl_idx += 8;
	}

	// read chr 1 next, offset 32
	data -= 64;
	pxl_idx = 136;
	for(u8 row = 0; row < 32; row += 4)
	{
		for(u8 shift = 0; shift < 8; shift++)
		{
			this_pxl = 0;
			if((data[row] >> shift) & 1) this_pxl |= 1;
			if((data[row + 1] >> shift) & 1) this_pxl |= 2;
			if((data[row + 2] >> shift) & 1) this_pxl |= 4;
			if((data[row + 3] >> shift) & 1) this_pxl |= 8;
			_out[pxl_idx++] = this_pxl;
		}
		pxl_idx += 8;
	}
	return _out;
}

// https://wiki.neogeodev.org/index.php?title=Sprite_graphics_format#Coding
// https://wiki.neogeodev.org/index.php?title=Palettes
// https://wiki.neogeodev.org/index.php?title=Colors
// tile order: 0 (upper right), 1 (lower right), 2 (upper left), 3 (lower left)
// CD SPR files: bitplane 1->0->3->2 (which is to say, little endian)

// reading bits:
// t2 (data + 64bytes)
// t0 (data)
// t3 (data + 96bytes)
// t1 (data + 32bytes)

const chr* snk_neogeocd_cx::get_chr(const u8* data)
{
	return get_chr_neogeocd(data);
}

const chr* snk_neogeocd_cx::get_chr_neogeocd(const u8* data)
{
	auto _out = new chr[CHR_PXL_COUNT];
	u8 this_pxl, pxl_idx{0};
	// read chr 2 first, offset 64 bytes
	data += 64;
	for(u8 row = 0; row < 32; row += 4)
	{
		for(u8 shift = 0; shift < 8; shift++)
		{
			this_pxl = 0;
			if((data[row + 1] >> shift) & 1) this_pxl |= 1;
			if((data[row] >> shift) & 1) this_pxl |= 2;
			if((data[row + 3] >> shift) & 1) this_pxl |= 4;
			if((data[row + 2] >> shift) & 1) this_pxl |= 8;
			_out[pxl_idx++] = this_pxl;
		}
		pxl_idx += 8;
	}

	// read chr 0 next, offset 0
	data -= 64;
	pxl_idx = 8;
	for(u8 row = 0; row < 32; row += 4)
	{
		for(u8 shift = 0; shift < 8; shift++)
		{
			this_pxl = 0;
			if((data[row + 1] >> shift) & 1) this_pxl |= 1;
			if((data[row] >> shift) & 1) this_pxl |= 2;
			if((data[row + 3] >> shift) & 1) this_pxl |= 4;
			if((data[row + 2] >> shift) & 1) this_pxl |= 8;
			_out[pxl_idx++] = this_pxl;
		}
		pxl_idx += 8;
	}

	// read chr 3 next, offset 96
	data += 96;
	pxl_idx = 128;
	for(u8 row = 0; row < 32; row += 4)
	{
		for(u8 shift = 0; shift < 8; shift++)
		{
			this_pxl = 0;
			if((data[row + 1] >> shift) & 1) this_pxl |= 1;
			if((data[row] >> shift) & 1) this_pxl |= 2;
			if((data[row + 3] >> shift) & 1) this_pxl |= 4;
			if((data[row + 2] >> shift) & 1) this_pxl |= 8;
			_out[pxl_idx++] = this_pxl;
		}
		pxl_idx += 8;
	}

	// read chr 1 next, offset 32
	data -= 64;
	pxl_idx = 136;
	for(u8 row = 0; row < 32; row += 4)
	{
		for(u8 shift = 0; shift < 8; shift++)
		{
			this_pxl = 0;
			if((data[row + 1] >> shift) & 1) this_pxl |= 1;
			if((data[row] >> shift) & 1) this_pxl |= 2;
			if((data[row + 3] >> shift) & 1) this_pxl |= 4;
			if((data[row + 2] >> shift) & 1) this_pxl |= 8;
			_out[pxl_idx++] = this_pxl;
		}
		pxl_idx += 8;
	}
	return _out;
}

}	// namespace chrgfx