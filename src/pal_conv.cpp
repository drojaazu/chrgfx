#include "pal_conv.hpp"
#include "utils.hpp"
#include <algorithm>

namespace chrgfx
{
/**
 * Returns a standard color palette
 */
palette *get_pal(pal_def &paldef, bptr data,
								 color (*get_color)(pal_def &, u32 rawvalue), s16 subpal_idx)
{
	/* abandon hope, all who enter here
			this algorithm is a ~mess~ right now, as I wedged in support for weird
			cases (LOOKIN' AT YOU, VIRTUAL BOY PALETTES)
			the psuedo code below is only partially valid anymore
			needs a serious cleanup, but that's for another branch, another day...
	*/

	/*
	psuedo:
	 Loop on number of bytes for a palette
	 Palette is either full sys palette or subpalette
		get size of a single subpalette:
			- defined in paldef? if so use that
			- if not defined: size is ((bits/entry * entries/subpal) / 8) [if 0, then
	= 1] get data size of output palette:
			- using subpalette? size is same as datasize of single subpal
			- else: size is (subpal_datasize * subpals/fullpal)
	/ 8 bits per entry = entry_datasize % 8 bytes per entry = (entry_datasize / 8)
	+ (bits per entry > 0 ? 1 : 0)

		for(data size of palette)
			if(bytes per entry == 1 && bits > 0)
				if subpal size > bytes per entry
					apply endianness


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
	if(subpal_idx >= paldef.get_subpal_count()) {
		throw std::invalid_argument(
				"Specified subpalette index is invalid for given pal_def");
	}

	u8 workentry_bitsize = paldef.get_entry_datasize() % 8;
	u8 workentry_bytesize =
			(paldef.get_entry_datasize() / 8) + (workentry_bitsize ? 1 : 0);

	u16 subpal_datasize = paldef.get_subpal_datasize();
	if(subpal_datasize == 0) {
		if(workentry_bytesize == 1 && workentry_bitsize > 0) {
			subpal_datasize = workentry_bitsize * paldef.get_subpal_length();
		} else {
			subpal_datasize = (workentry_bytesize * 8) * paldef.get_subpal_length();
		}
	}
	u16 temp = (subpal_datasize / 8);

	// total size of full palette in bytes
	u16 fullpal_datasize{0};
	if(subpal_idx >= 0) {
		// if we're using a subpalette
		// fullpal_datasize =
		//		(paldef.get_entry_datasize() * paldef.get_subpal_length()) / 8;
		fullpal_datasize = subpal_datasize / 8;
		data += (fullpal_datasize * subpal_idx);
	} else {
		// if we're using the full palette
		// limit the total number of entries in the final palette to 256
		u16 entry_count =
				(((paldef.get_subpal_count() * paldef.get_subpal_length()) > 256)
						 ? 256
						 : paldef.get_subpal_count() * paldef.get_subpal_length());
		fullpal_datasize = (paldef.get_entry_datasize() * entry_count) / 8;
	}

	u32 bitmask = create_bitmask32(paldef.get_entry_datasize());

	palette *out = new palette();
	u8 *(*copyfunc)(bptr, bptr, u8 *);

	if(bigend_sys == paldef.get_is_big_endian()) {
		copyfunc = std::copy;
	} else {
		copyfunc = std::reverse_copy;
	}

	u16 data_inc{0};
	if(workentry_bytesize == 1 && workentry_bitsize > 0 &&
		 temp > workentry_bytesize) {
		data_inc = temp;
	} else {
		data_inc = workentry_bytesize;
	}

	for(u16 data_iter{0}; data_iter < fullpal_datasize; data_iter += data_inc) {
		if(workentry_bytesize == 1 && workentry_bitsize > 0) {
			// color entry is less than a full byte
			u32 dataval{0};
			if(temp > workentry_bytesize) {
				u8 temparr[temp];
				copyfunc(data + data_iter, data + (data_iter + temp), temparr);
				u32 rawcolor{*reinterpret_cast<u32 *>(temparr)};
				dataval = rawcolor;
			} else {
				dataval = data[data_iter];
			}
			for(u8 shift_iter{0}; shift_iter < (8 / workentry_bitsize);
					++shift_iter) {
				out->push_back(paldef.get_syspal()->at(
						(dataval >> (workentry_bitsize * shift_iter)) & bitmask));
			}
		} else {
			// color entry is one full byte or more
			u8 temparr[workentry_bytesize];
			copyfunc(data + data_iter, data + (data_iter + workentry_bytesize),
							 temparr);

			u32 rawcolor{*reinterpret_cast<u32 *>(temparr)};
			rawcolor &= bitmask;
			out->push_back(get_color(paldef, rawcolor));
		}
	}
	return out;
};

palette *get_pal_refpal(pal_def &paldef, bptr data, s16 subpal_idx)
{
	return get_pal(
			paldef, data,
			[](pal_def &paldef, u32 rawvalue) -> color {
				return paldef.get_syspal()->at(rawvalue);
			},
			subpal_idx);
}

palette *get_pal_coldef(pal_def &paldef, bptr data, s16 subpal_idx)
{
	return get_pal(
			paldef, data,
			[](pal_def &paldef, u32 rawvalue) -> color {
				return calc_color(*paldef.get_coldef(), rawvalue);
			},
			subpal_idx);
};

palette *get_pal_tlp(pal_def &paldef, bptr data, s16 subpal_idx)
{
	if(data[0] != 0x54 || data[1] != 0x50 || data[2] != 0x4c)
		std::cerr << "Warning: Does not appear to be a valid TLP palette"
							<< std::endl;

	if(data[3] != 0) {
		throw std::invalid_argument("Unsupported TPL type (only RGB supported)");
	}

	u16 pal_length = paldef.get_subpal_count() * paldef.get_subpal_length();

	// start at the actual data
	data += 4;
	if(subpal_idx >= 0) {
		pal_length = paldef.get_subpal_length();
		data += (subpal_idx * paldef.get_subpal_length() * 3);
	}

	if(pal_length > 256) {
		throw std::invalid_argument("Specified TPL size is invalid (subpal_count * "
																"subpal_length cannot be greater than 256)");
	}
	auto out = new palette();
	out->reserve(pal_length);

	for(size_t palEntryIter = 0; palEntryIter < pal_length; ++palEntryIter) {
		out->push_back(color(data[0], data[1], data[2]));
		data += 3;
	}
	return out;
};

color calc_color(col_def &coldef, u32 data)
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
	for(u16 pass_iter = 0; pass_iter < coldef.get_color_passes(); ++pass_iter) {
		bitmask = create_bitmask8(coldef.get_red_bitcount()[pass_iter]);
		red |= ((data >> coldef.get_red_shift()[pass_iter]) & bitmask)
					 << red_bitcount;
		red_bitcount += coldef.get_red_bitcount()[pass_iter];

		bitmask = create_bitmask8(coldef.get_green_bitcount()[pass_iter]);
		green |= ((data >> coldef.get_green_shift()[pass_iter]) & bitmask)
						 << green_bitcount;
		green_bitcount += coldef.get_green_bitcount()[pass_iter];

		bitmask = create_bitmask8(coldef.get_blue_bitcount()[pass_iter]);
		blue |= ((data >> coldef.get_blue_shift()[pass_iter]) & bitmask)
						<< blue_bitcount;
		blue_bitcount += coldef.get_blue_bitcount()[pass_iter];
	}

	red = expand_bits(red, red_bitcount);
	green = expand_bits(green, green_bitcount);
	blue = expand_bits(blue, blue_bitcount);

	return color(red, green, blue);
};

} // namespace chrgfx