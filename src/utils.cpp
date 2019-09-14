#include "utils.hpp"

using namespace png;

namespace chrgfx
{
/**
 * Expands bits to fill out a full byte.
 */
u8 expand_bits(u8 data, u8 bitcount)
{
	// shamelessly stolen from MAME
	if(bitcount == 1)
	{
		return (data & 1) ? 0xff : 0x00;
	}
	if(bitcount == 2)
	{
		data &= 3;
		return (data << 6) | (data << 4) | (data << 2) | data;
	}
	if(bitcount == 3)
	{
		data &= 7;
		return (data << 5) | (data << 2) | (data >> 1);
	}
	if(bitcount == 4)
	{
		data &= 0xf;
		return (data << 4) | data;
	}
	if(bitcount == 5)
	{
		data &= 0x1f;
		return (data << 3) | (data >> 2);
	}
	if(bitcount == 6)
	{
		data &= 0x3f;
		return (data << 2) | (data >> 4);
	}
	if(bitcount == 7)
	{
		data &= 0x7f;
		return (data << 1) | (data >> 6);
	}
	return data;
}

/**
 * Returns a value with set bits equivalent to the number of bits requested
 * e.g. 5 bits = 0x0000001F (0x1F = 00011111)
 */
u32 create_bitmask32(u8 bitcount)
{
	// max 32 bits, anything higher would waste cycles
	if(bitcount > 31)
	{
		return 0xffffffff;
	}

	u32 bitmask{0};
	for(s16 mask_iter{0}; mask_iter < bitcount; ++mask_iter)
	{
		bitmask |= (bitmask << 1) | 1;
	}
	return bitmask;
}

u16 create_bitmask16(u8 bitcount)
{
	if(bitcount > 15)
	{
		return 0xffff;
	}

	u16 bitmask{0};
	for(s16 mask_iter{0}; mask_iter < bitcount; ++mask_iter)
	{
		bitmask |= (bitmask << 1) | 1;
	}
	return bitmask;
}

u8 create_bitmask8(u8 bitcount)
{
	if(bitcount > 7)
	{
		return 0xff;
	}

	u8 bitmask{0};
	for(s16 mask_iter{0}; mask_iter < bitcount; ++mask_iter)
	{
		bitmask |= (bitmask << 1) | 1;
	}
	return bitmask;
}

palette* make_pal(bool blank)
{
	auto outpal = new palette();
	outpal->reserve(256);

	if(blank)
	{
		outpal->insert(outpal->begin(), 256, color(0, 0, 0));
	}
	else
	{
		// basic 16 color palette based on Xterm colors
		// repeated 16x for 256 entry 8bpp palette
		for(uint8_t l = 0; l < 16; l++)
		{
			outpal->push_back(color(0, 0, 0));
			outpal->push_back(color(128, 0, 0));
			outpal->push_back(color(0, 128, 0));
			outpal->push_back(color(128, 128, 0));

			outpal->push_back(color(0, 0, 128));
			outpal->push_back(color(128, 0, 128));
			outpal->push_back(color(0, 128, 128));
			outpal->push_back(color(192, 192, 192));

			outpal->push_back(color(128, 128, 128));
			outpal->push_back(color(255, 0, 0));
			outpal->push_back(color(0, 255, 0));
			outpal->push_back(color(255, 255, 0));

			outpal->push_back(color(0, 0, 255));
			outpal->push_back(color(255, 0, 255));
			outpal->push_back(color(0, 255, 255));
			outpal->push_back(color(255, 255, 255));
		}
	}
	return outpal;
}

// fill in any blank entries in a palette to bring it up to 256
void fill_pal(palette* pal)
{
	if(pal->size() >= 256) return;

	u8 toFill = 256 - pal->size();

	for(u8 fillIter = 0; fillIter < toFill; fillIter++)
		pal->push_back(color(0, 0, 0));

	return;
}

}	// namespace chrgfx