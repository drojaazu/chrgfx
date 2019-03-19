#include "nintendo_vb.hpp"

using namespace png;

namespace chrgfx
{
// ----------------- CHR
const chr_traits nintendo_vboy_cx::traits = std_2bpp_tile;

const chr_traits* nintendo_vboy_cx::get_traits()
{
	return &nintendo_vboy_cx::traits;
}

const u8 CHR_PXL_COUNT = 64;

const chr* nintendo_vboy_cx::get_chr(const u8* data)
{
	return get_chr_vboy(data);
}

const chr* nintendo_vboy_cx::get_chr_vboy(const u8* data)
{
	auto _out = new chr[CHR_PXL_COUNT];
	u8 pxl_idx{0};

	for(u8 row = 0; row < 16; row++)
	{
		for(u8 shift = 0; shift <= 6; shift += 2)
		{
			_out[pxl_idx++] = (data[row] >> shift) & 3;
		}
	}

	return _out;
}

// ----------------- PALETTES
// 4 colors in each palette, 2 bits per color
// 1 byte per palette
// stored starting at 0x5f860, 8 palettes total (4 for BG, 4 for OBJ)
// each palette is stored in two bytes, even though each pal is 1 byte
// the palette is mirrored across the two bytes
const pal_traits nintendo_vboy_px::traits{32, 1, 8, 4};

const pal_traits* nintendo_vboy_px::get_traits() { return &traits; }

// colors can vary depending on the values of BRTA/BRTB/BRTC
// this is a rendition of a 'normal' color set
const color nintendo_vboy_px::vboy_colors[] = {
		color(0, 0, 0), color(0x55, 0, 0), color(0xaa, 0, 0), color(0xff, 0, 0)};

const color* nintendo_vboy_px::get_rgb(const u8* data)
{
	if(*data > 3) throw std::out_of_range("Invalid VirtualBoy palette entry");
	return &vboy_colors[*data];
}

const palette* nintendo_vboy_px::get_pal(const u8* data, s16 subpal)
{
	auto _out = new palette();
	u8 paliter{1}, count{16};

	if(subpal > 7)
	{
		std::cerr << "Warning: invalid subpalette specified; using full palette"
							<< std::endl;
		subpal = -1;
	}

	if(subpal < 0)
	{
		_out->reserve(32);
	}
	else
	{
		_out->reserve(4);
		paliter = (subpal * 2) + 1;
		count = paliter + 2;
	}

	// 8 bits, every 2 bits is a palette entry
	// 3  2  1  0   pal entry
	// 11 11 11 11  bit
	for(; paliter < count; paliter += 2)
	{
		for(u8 shift = 0; shift < 8; shift += 2)
		{
			_out->push_back(vboy_colors[(data[paliter] >> shift) & 3]);
		}
	}

	return _out;
}

}	// namespace chrgfx