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

// https://wiki.neogeodev.org/index.php?title=Sprite_graphics_format#Coding
// https://wiki.neogeodev.org/index.php?title=Palettes
// https://wiki.neogeodev.org/index.php?title=Colors
// chr order: 0 (upper right), 1 (lower right), 2 (upper left), 3 (lower left)
// C ROMs: bitplane 0->1->2->3
// CD SPR files: bitplane 1->0->3->2 (which is to say, little endian)

// reading bits:
// t2 (data + 64bytes)
// t0 (data)
// t3 (data + 96bytes)
// t1 (data + 32bytes)

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
	// set position 8 pixels away (to account for the first chr drawn, on the
	// left)
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
	// set position past the data for the two chrs already drawn (16x8 pixels =
	// 128 bytes)
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
	// set position past two chrs + 8 pixels for the chr on the left
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

const chr* snk_neogeocd_cx::get_chr(const u8* data)
{
	return get_chr_neogeocd(data);
}

const chr* snk_neogeocd_cx::get_chr_neogeocd(const u8* data)
{
	// same as C ROMs, but each word is little endian
	auto _out = new chr[CHR_PXL_COUNT];
	u8 this_pxl, pxl_idx{0};
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

// ----------------- PALETTES
// 4096 colors, 16 colors per palette * 256 palettes
const pal_traits snk_neogeo_px::traits{4096, 2, 256, 16};

const pal_traits* snk_neogeo_px::get_traits() { return &traits; }

const color* snk_neogeo_px::get_rgb(const u8* data)
{
	return get_rgb_neogeo(data);
}

const color* snk_neogeo_px::get_rgb_neogeo(const u8* data)
{
	// 15 |                               |0
	//    DDR0G0B0R4R3R2R1 G4G3G2G1B4B3B2B1
	// D = "dark bit", shared LSB of all colors
	// big endian

	// LSBs in first byte
	u8 r = (data[0] & 0x40) >> 5;
	u8 g = (data[0] & 0x20) >> 4;
	u8 b = (data[0] & 0x10) >> 3;

	// remaining bits
	r |= (data[0] & 0x0f) << 2;
	g |= (data[1] & 0xf0) >> 2;
	b |= (data[1] & 0x0f) << 2;

	// dark bit
	r |= (data[0] & 0x80) >> 7;
	g |= (data[0] & 0x80) >> 7;
	b |= (data[0] & 0x80) >> 7;

	// shift for brightness
	r = (r << 2);
	g = (g << 2);
	b = (b << 2);

	return new color(r, g, b);
}

const palette* snk_neogeo_px::get_pal(const u8* data, s16 subpal)
{
	return chrgfx::get_pal(this, data, subpal);
}
}	// namespace chrgfx