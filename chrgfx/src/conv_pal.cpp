/********************************
 * conv_pal
 * Convert palette formats
 ********************************/
#include "conv_pal.hpp"

namespace chrgfx
{

png::palette from_pal(paldef const &from_paldef, coldef const &from_coldef,
											u8 const *data, s16 const subpal_idx)
{
	u16 const subpal_count{from_paldef.get_subpal_count()};
	if(subpal_idx >= subpal_count) {
		throw std::invalid_argument("Specified subpalette index is invalid");
	}

	u16 entry_datasize{from_paldef.get_entry_datasize()},
			subpal_length{from_paldef.get_subpal_length()},
			subpal_datasize{from_paldef.get_subpal_datasize()};

	if(subpal_datasize == 0) {
		subpal_datasize = entry_datasize * subpal_length;
	}

	u32 entry_mask = create_bitmask32(entry_datasize);

	u16 pal_length{0};

	u8 const *data_ptr;

	if(subpal_idx >= 0) {
		// if we're using a subpalette
		pal_length = from_paldef.get_subpal_length();
		data_ptr = data + ((subpal_datasize * subpal_idx) / 8);
	} else {
		// if we're using the full palette
		// limit the total number of entries in the final palette to 256
		pal_length = (((subpal_count * from_paldef.get_subpal_length()) > 256)
											? 256
											: subpal_count * from_paldef.get_subpal_length());
		data_ptr = data;
	}

	palette out;
	out.reserve(pal_length);

	// used to copy the color entry bytes into a temp array to be cast as a u32
	u8 *(*copyfunc)(u8 const *, u8 const *, u8 *);
	if(bigend_sys == from_coldef.get_is_big_endian()) {
		copyfunc = std::copy;
	} else {
		copyfunc = std::reverse_copy;
	}
	size_t bit_align_ptr{0},
			temp_buff_size{
					(size_t)((entry_datasize / 8) + (entry_datasize % 8 > 0 ? 1 : 0))},
			byte_offset{0}, bit_offset{0};

	u8 temp_buff[temp_buff_size];

	u32 this_entry{0};

	// loop for each color entry in the output palette
	for(u16 pal_entry; pal_entry < pal_length; ++pal_entry) {
		byte_offset = bit_align_ptr / 8;
		bit_offset = bit_align_ptr % 8;

		copyfunc(data_ptr + byte_offset, data_ptr + byte_offset + temp_buff_size,
						 temp_buff);

		this_entry = *reinterpret_cast<u32 *>(temp_buff);
		this_entry >>= bit_offset;
		this_entry &= entry_mask;

		out.push_back(from_color(from_coldef, this_entry));

		bit_align_ptr += entry_datasize;

		// for special cases where the data size of the subpalette
		// is larger than the sum of the datasize of the colors
		// position the data ptr to the next actual subpalette
		if(pal_entry > 0 && pal_entry % from_paldef.get_subpal_length() == 0) {
			bit_align_ptr +=
					subpal_datasize - (entry_datasize * from_paldef.get_subpal_length());
		}
	}

	return out;
}

// to/from color functions do not take endianness into account
// that should be done before/after!

png::color from_color(coldef const &from_coldef, u32 const data)
{

	if(from_coldef.use_refpal()) {
		return from_coldef.get_refpal_entry(data);
	} else {

		/*
	psuedo:
	-u32 data
	-for each pass
		-shift red, apply mask, OR with R var
		-shift green, apply mask, OR with G var
		-shift blue, apply mask, OR with B var
	-expand R, G, B

	 */
		u8 red{0}, green{0}, blue{0};
		u8 red_bitcount{0}, green_bitcount{0}, blue_bitcount{0};
		u8 bitmask{0};
		for(rgb_layout const &this_pass : from_coldef.get_rgb_layout()) {
			bitmask = create_bitmask8(this_pass.get_red_count());
			red |= ((data >> this_pass.get_red_shift()) & bitmask) << red_bitcount;
			red_bitcount += this_pass.get_red_count();

			bitmask = create_bitmask8(this_pass.get_green_count());
			green |= ((data >> this_pass.get_green_shift()) & bitmask)
							 << green_bitcount;
			green_bitcount += this_pass.get_green_count();

			bitmask = create_bitmask8(this_pass.get_blue_count());
			blue |= ((data >> this_pass.get_blue_shift()) & bitmask) << blue_bitcount;
			blue_bitcount += this_pass.get_blue_count();
		}

		red = expand_bits(red, red_bitcount);
		green = expand_bits(green, green_bitcount);
		blue = expand_bits(blue, blue_bitcount);

		return color(red, green, blue);
	}
};

u32 to_color(coldef const &to_coldef, png::color const data)
{
	if(to_coldef.use_refpal()) {
		// if we're using the refpal as intended
		// (a system palette)
		// each entry should be unique
		// so...

		// TODO: this is not going to work
		// need to find nearest color in refpal
		// see https://en.wikipedia.org/wiki/Color_difference
		// use formula to compare against each color in palette, find the one
		// closest
		return to_coldef.get_refpal_idx(data);
	} else {
		/*
			seperate r g b from color

			for each channel, reduce to the number of bits available


			track shift for individual color (increases by size each pass)
			for each pass:
			 red:
				- create bitmask based on size
				- shift bitmask
				- AND bitmask to red
				- shift masked color back to 0
				- shift left as defined, OR on to out value
			green:
			blue:
			 as above
		*/

		u32 out{0};
		// TEMPORARY - need to full implement bitdepth field in coldef for proper
		// bit size!
		u8 bitdepth = to_coldef.get_rgb_layout()[0].get_red_count();
		u8 red{dither_bits(data.red, bitdepth)}, red_pass_shift{0},
				green{dither_bits(data.green, bitdepth)}, green_pass_shift{0},
				blue{dither_bits(data.blue, bitdepth)}, blue_pass_shift{0};

		u8 bitmask{0};
		u32 temp;

		for(rgb_layout const &this_pass : to_coldef.get_rgb_layout()) {
			bitmask = (create_bitmask8(this_pass.get_red_count())) << red_pass_shift;
			temp = (red & bitmask);
			temp = temp << this_pass.get_red_shift();
			out |= temp;
			red_pass_shift += this_pass.get_red_count();

			bitmask = (create_bitmask8(this_pass.get_green_count()))
								<< green_pass_shift;
			temp = (green & bitmask) << this_pass.get_green_shift();
			out |= temp;
			green_pass_shift += this_pass.get_green_count();

			bitmask = (create_bitmask8(this_pass.get_blue_count()))
								<< blue_pass_shift;
			temp = (blue & bitmask) << this_pass.get_blue_shift();
			out |= temp;
			blue_pass_shift += this_pass.get_blue_count();
		}

		return out;
	}
}

u8 *to_pal(paldef const &to_paldef, coldef const &to_coldef,
					 png::palette const data, s16 const subpal_idx)
{
	/*
		determine full palette or subpal
		set size of output based on above
		for each entry

	*/

	u16 const subpal_count{to_paldef.get_subpal_count()};
	if(subpal_idx >= subpal_count) {
		throw std::invalid_argument("Specified subpalette index is invalid");
	}

	u16 entry_datasize{to_paldef.get_entry_datasize()},
			entry_datasize_bytes{(u16)(to_paldef.get_entry_datasize() / 8)},
			subpal_length{to_paldef.get_subpal_length()},
			subpal_datasize{to_paldef.get_subpal_datasize()};

	// datasize is only manually specified in cases where the color data does not
	// actually fill the whole subpalette dataspace
	// nintendo virtual boy is the main culprit for this
	if(subpal_datasize == 0) {
		subpal_datasize = entry_datasize * subpal_length;
	}

	u32 const entry_mask{create_bitmask32(entry_datasize)};

	u16 pal_length{0};

	size_t data_ptr{0};

	if(subpal_idx >= 0) {
		// if we're using a subpalette
		pal_length = subpal_length;
		data_ptr = subpal_length * subpal_idx;
	} else {
		// if we're using the full palette
		// limit the total number of entries in the final palette to 256
		pal_length =
				(((subpal_count * subpal_length) > 256) ? 256
																								: subpal_count * subpal_length);
	}

	int out_datasize{pal_length * (entry_datasize / 8)};
	u8 *out{new u8[out_datasize]};
	std::fill(out, out + out_datasize, 0);

	size_t bit_align_ptr{0}, byte_offset{0}, bit_offset{0};

	u8 entry_temp[entry_datasize_bytes];

	// for every color entry in the palette
	// convert that color into the coldef'ed version
	// mask away the extra bits
	// if entry size does not align to bytes, pack entries ***
	// resize to subpal data size if necessary ***
	u8 *(*copyfunc)(u8 *, u8 *, u8 *);
	if(bigend_sys == to_coldef.get_is_big_endian()) {
		copyfunc = std::copy;
	} else {
		copyfunc = std::reverse_copy;
	}

	for(u16 pal_entry; pal_entry < pal_length; ++pal_entry) {
		byte_offset = bit_align_ptr / 8;
		bit_offset = bit_align_ptr % 8;

		u32 this_color = to_color(to_coldef, data.at(pal_entry));
		// color data stored in lsb
		this_color &= entry_mask;
		if(entry_datasize < 8) {
			// crazy cases here
			// big fat ugly TODO here
			throw std::invalid_argument("Unimplemented, ya dingus");
		} else {
			// nice, easy cases here
			for(size_t s{0}; s < entry_datasize_bytes; ++s) {
				entry_temp[s] = (this_color >> (s * 8)) & 0xff;
			}
			copyfunc(entry_temp, entry_temp + entry_datasize_bytes,
							 out + byte_offset);
		}
		bit_align_ptr += entry_datasize;
	}

	return out;
}

palette *get_pal_tlp(paldef const &paldef, u8 *data, s16 subpal_idx)
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

} // namespace chrgfx