#include "nintendo_fc_gfx.hpp"

using namespace png;

namespace gfx
{
// -------- CHR
const chr_traits nintendo_fc_cx::traits = std_2bpp_tile;

const chr_traits* nintendo_fc_cx::get_traits()
{
	return &nintendo_fc_cx::traits;
}

const chr* nintendo_fc_cx::get_chr(BYTE* data) { return get_chr_nes(data); }

const chr* nintendo_fc_cx::get_chr_nes(BYTE* data)
{
	auto _out = new chr(traits.width, traits.height);
	size_t pxlRowCount = 0;
	BYTE thisPxl;
	auto thisPxlRow = std::vector<index_pixel>();

	for(size_t row = 0; row < 8; row ++)
	{
		thisPxlRow.clear();
		thisPxlRow.reserve(traits.width);
		for(int8_t shift = 7; shift >= 0; shift--)
		{
			thisPxl = 0;
			if((data[row] >> shift) & 1) thisPxl++;
			if((data[row + 1] >> shift) & 1) thisPxl += 2;
			thisPxlRow.push_back(thisPxl);
		}
		_out->put_row(pxlRowCount, thisPxlRow);
		pxlRowCount++;
	}

	return _out;
}



// -------- PAL
// 64 colors
const pal_traits nintendo_fc_px::traits = {64, 0};


const pal_traits* nintendo_fc_px::get_traits() {
	return &nintendo_fc_px::traits;
}

// a 'standard' 2C02 PPU palette
// https://wiki.nesdev.com/w/index.php/PPU_palettes
const palette nintendo_fc_px::std_fc_pal = palette {
    color(84,84,84),    // 0x0
    color(0,30,116),
    color(8,16,44),
    color(48,0,136),
    color(68,0,100),
    color(92,0,48),
    color(84,4,0),
    color(60,24,0),
    color(32,42,0),
    color(8,58,0),
    color(0,64,0),
    color(0,60,0),
    color(0,50,60),
    color(0,0,0),
		color(0,0,0),
		color(0,0,0),
    color(152,150,152), // 0x10
    color(8,76,196),
    color(48,50,236),
    color(92,30,228),
    color(136,20,176),
    color(160,20,100),
    color(152,34,32),
    color(120,60,0),
    color(84,90,0),
    color(40,114,0),
    color(8,124,0),
    color(0,118,40),
    color(0,102,120),
    color(0,0,0),
		color(0,0,0),
		color(0,0,0),
    color(236,238,236), // 0x20
		color(76,154,236),
		color(120,124,236),
		color(176,98,236),
		color(228,84,236),
		color(236,88,180),
		color(236,106,100),
		color(212,136,32),
		color(160,170,0),
		color(116,196,0),
		color(76,208,32),
		color(56,204,108),
		color(56,180,204),
		color(60,60,60),
		color(0,0,0),
		color(0,0,0),
		color(236,238,236),	// 0x30
		color(168,204,236),
		color(188,188,236),
		color(212,178,236),
		color(236,174,236),
		color(236,174,212),
		color(236,180,176),
		color(228,196,144),
		color(204,210,120),
		color(180,222,120),
		color(168,226,144),
		color(152,226,180),
		color(160,214,228),
		color(160,162,160),
		color(0,0,0),
		color(0,0,0)
};

const color* nintendo_fc_px::get_rgb(BYTE* data)
{
	if(*data > 0x40)
		throw std::out_of_range("Invalid NES palette entry");
	return &std_fc_pal.at(*data);
}

const palette* nintendo_fc_px::get_pal()
{
	// ignore whatever is passed in
	return nintendo_fc_px::get_pal(nullptr);
}

const palette* nintendo_fc_px::get_pal(BYTE* data)
{
	// ignore whatever is passed in
	return &nintendo_fc_px::std_fc_pal;
}
}