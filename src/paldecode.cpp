#include "paldecode.hpp"
#include <algorithm>
#include "utils.hpp"

namespace chrgfx
{
const palette* pal_decode(const pal_def* paldef, const u8* data,
													color (*get_color)(const pal_def*,
																						 const u32 rawvalue),
													s16 subpal_idx)
{
	/*
	psuedo:
	 Loop on number of bytes for a full palette
		data size of palette = (bits/entry * entries/subpal * subpals/fullpal) / 8
		bits per entry = entry size % 8
		bytes per entry = (entry size / 8) + (bits per entry > 0 ? 1 : 0)

		for(data size of palette)
			if(bytes per entry == 1 && bits > 0)
				for(8 / bits per entry)
					data >> (iter * bits per entry)
					apply bitmask
					push value to palette
			else
				read 'bytes per entry' of data into temp array (reverse if endianness
	does not match)
				convert array to u32
				apply bitmask
				push value to palette

	 */
	if(subpal_idx >= paldef->subpal_count)
	{
		throw "Specified subpalette index is invalid for given pal_def";
	}

	u8 workentry_bitsize = paldef->entry_datasize % 8;
	u8 workentry_bytesize =
			(paldef->entry_datasize / 8) + (workentry_bitsize ? 1 : 0);

	// total size of full palette in bytes
	u16 fullpal_datasize{0};
	if(subpal_idx >= 0)
	{
		// if we're using a subpalette
		fullpal_datasize = (paldef->entry_datasize * paldef->subpal_length) / 8;
		data += (fullpal_datasize * subpal_idx);
	}
	else
	{
		// if we're using the full palette
		// limit the total number of entries in the final palette to 256
		u16 entry_count = (((paldef->subpal_count * paldef->subpal_length) > 256)
													 ? 256
													 : paldef->subpal_count * paldef->subpal_length);
		fullpal_datasize = (paldef->entry_datasize * entry_count) / 8;
	}

	u32 bitmask = create_bitmask32(paldef->entry_datasize);

	palette* out = new palette();
	u8* (*copyfunc)(const u8*, const u8*, u8*);

	if(is_system_bigendian() == paldef->is_big_endian)
	{
		copyfunc = std::copy;
	}
	else
	{
		copyfunc = std::reverse_copy;
	}

	for(u16 data_iter{0}; data_iter < fullpal_datasize;
			data_iter += workentry_bytesize)
	{
		if(workentry_bytesize == 1 && workentry_bitsize > 0)
		{
			// color entry is less than a full byte
			u32 dataval{data[data_iter]};
			for(u8 shift_iter{0}; shift_iter < (8 / workentry_bitsize); ++shift_iter)
			{
				out->push_back(paldef->syspal->at(
						(dataval >> (workentry_bitsize * shift_iter)) & bitmask));
			}
		}
		else
		{
			u8 temparr[workentry_bytesize];
			copyfunc(data + data_iter, data + (data_iter + workentry_bytesize),
							 temparr);

			u32 rawcolor = *reinterpret_cast<u32*>(temparr);
			rawcolor &= bitmask;
			color t = get_color(paldef, rawcolor);
			out->push_back(t);
		}
	}
	return out;
};

const palette* pal_decode_fixed(const pal_def* paldef, const u8* data,
																const s16 subpal_idx)
{
	return pal_decode(
			paldef, data,
			[](const pal_def* paldef, const u32 rawvalue) -> color {
				return paldef->syspal->at(rawvalue);
			},
			subpal_idx);
}

const palette* pal_decode_calc(const pal_def* paldef, const u8* data,
															 const s16 subpal_idx)
{
	return pal_decode(
			paldef, data,
			[](const pal_def* paldef, const u32 rawvalue) -> color {
				return calc_color(paldef->colordef, rawvalue);
			},
			subpal_idx);
};

const palette* pal_decode_soft_tlp(const pal_def* paldef, const u8* data,
																	 const s16 subpal_idx)
{
	if(data[0] != 0x54 || data[1] != 0x50 || data[2] != 0x4c)
		std::cerr << "Warning: Does not appear to be a valid TLP palette"
							<< std::endl;

	switch(data[3])
	{
		case 0:
		{
			// rgb
			data += 4;
			// return palette_converter::get_palette_generic(this, data, 3, 256);
			auto out = new palette();
			out->reserve(256);

			for(size_t palEntryIter = 0; palEntryIter < 256; palEntryIter++)
			{
				out->push_back(color(data[0], data[1], data[2]));
				data += 3;
			}
			return out;
			break;
		}
		case 1:
		{
			// famicom
			throw "Type 1 TLP palettes currently unsupported";
			break;
		}
		case 2:
		{
			// sfc
			throw "Type 2 TLP palettes currently unsupported";
			break;
		}
	}
	throw "Invalid TLP palette type value";
};

color calc_color(const color_def* colordef, const u32 data)
{
	/*
psuedo:
-ptr to u32 data
-for each pass
	-shift red, apply mask, OR with R var
	-shift green, apply mask, OR with G var
	-shift blue, apply mask, OR with B var
-expand R, G, B

 */
	u8 red{0}, green{0}, blue{0};
	u8 red_bitcount{0}, green_bitcount{0}, blue_bitcount{0};
	u8 bitmask{0};
	for(u16 pass_iter = 0; pass_iter < colordef->color_passes; pass_iter++)
	{
		bitmask = create_bitmask8(colordef->red_bitcount[pass_iter]);
		red |= ((data >> colordef->red_shift[pass_iter]) & bitmask) << red_bitcount;
		red_bitcount += colordef->red_bitcount[pass_iter];

		bitmask = create_bitmask8(colordef->green_bitcount[pass_iter]);
		green |= ((data >> colordef->green_shift[pass_iter]) & bitmask)
						 << green_bitcount;
		green_bitcount += colordef->green_bitcount[pass_iter];

		bitmask = create_bitmask8(colordef->blue_bitcount[pass_iter]);
		blue |= ((data >> colordef->blue_shift[pass_iter]) & bitmask)
						<< blue_bitcount;
		blue_bitcount += colordef->blue_bitcount[pass_iter];
	}

	red = expand_bits(red, red_bitcount);
	green = expand_bits(green, green_bitcount);
	blue = expand_bits(blue, blue_bitcount);

	return color(red, green, blue);
};

}	// namespace chrgfx