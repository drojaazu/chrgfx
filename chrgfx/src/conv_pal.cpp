/********************************
 * conv_pal
 * Convert palette formats
 ********************************/
#include "conv_pal.hpp"

namespace chrgfx
{
namespace conv_pal
{

png::palette from_paldef_palette(paldef const &from_paldef,
																 coldef const &from_coldef, u8 const *data,
																 signed int const subpal_idx)
{
	u16 const subpal_count{from_paldef.get_subpal_count()};
	if(subpal_idx >= subpal_count) {
		throw std::invalid_argument("Specified subpalette index is invalid");
	}

	unsigned int const
			// size of one entry in the paldef palette (in bits)
			inpal_entry_datasize{from_paldef.get_entry_datasize()},
			// number of entries in a subpalette
			subpal_length{from_paldef.get_subpal_length()},
			// total size of a paldef subpalette (in bits)
			subpal_datasize{from_paldef.get_subpal_datasize() == 0
													? (inpal_entry_datasize * subpal_length)
													: from_paldef.get_subpal_datasize()};

	unsigned int outpal_length{0};

	u32 const inpal_entry_mask = create_bitmask32(inpal_entry_datasize);

	// points to the start of the palette data (for use with subpalettes)
	u8 const *inpal_data_ptr;

	// calculate the number of entries in our output palette
	if(subpal_idx >= 0) {
		// if we're using a subpalette
		outpal_length = from_paldef.get_subpal_length();
		inpal_data_ptr = data + ((subpal_datasize * subpal_idx) / 8);
	} else {
		// if we're using the full palette
		// limit the total number of entries in the final palette to 256
		outpal_length = (((subpal_count * from_paldef.get_subpal_length()) > 256)
												 ? 256
												 : subpal_count * from_paldef.get_subpal_length());
		inpal_data_ptr = data;
	}

	palette out;
	out.reserve(outpal_length);

	// used to copy the color entry bytes into a temp array to be cast as a u32
	u8 *(*copyfunc)(u8 const *, u8 const *, u8 *);
	if(bigend_sys == from_coldef.get_is_big_endian()) {
		copyfunc = std::copy;
	} else {
		copyfunc = std::reverse_copy;
	}

	size_t bit_align_ptr{0},
			temp_buff_size{(size_t)((inpal_entry_datasize / 8) +
															(inpal_entry_datasize % 8 > 0 ? 1 : 0))},
			byte_offset{0}, bit_offset{0};

	u8 temp_buff[temp_buff_size];

	u32 this_entry{0};

	// loop for each color entry in the output palette
	for(u16 pal_entry{0}; pal_entry < outpal_length; ++pal_entry) {
		byte_offset = bit_align_ptr / 8;
		bit_offset = bit_align_ptr % 8;

		// copy all data for one color entry into the temp buffer...
		copyfunc(inpal_data_ptr + byte_offset,
						 inpal_data_ptr + byte_offset + temp_buff_size, temp_buff);

		// recast that buffer (array) as a 32 bit value
		this_entry = *reinterpret_cast<u32 *>(temp_buff);
		this_entry >>= bit_offset;
		this_entry &= inpal_entry_mask;

		out.push_back(conv_col::from_coldef_color(from_coldef, this_entry));

		bit_align_ptr += inpal_entry_datasize;

		// for special cases where the data size of the subpalette
		// is larger than the sum of the datasize of the colors
		// position the data ptr to the next actual subpalette
		if(pal_entry > 0 && pal_entry % from_paldef.get_subpal_length() == 0) {
			bit_align_ptr += subpal_datasize -
											 (inpal_entry_datasize * from_paldef.get_subpal_length());
		}
	}

	return out;
}

u8 *to_paldef_palette(paldef const &to_paldef, coldef const &to_coldef,
											png::palette const data, signed int const subpal_idx)
{
	u16 const subpal_count{to_paldef.get_subpal_count()};
	if(subpal_idx >= subpal_count) {
		throw std::invalid_argument("Specified subpalette index is invalid");
	}
	unsigned int const entry_datasize{to_paldef.get_entry_datasize()},
			entry_datasize_bytes{(u16)(to_paldef.get_entry_datasize() / 8)},
			subpal_length{to_paldef.get_subpal_length()},
			subpal_datasize{to_paldef.get_subpal_datasize() == 0
													? entry_datasize * subpal_length
													: to_paldef.get_subpal_datasize()};

	unsigned int outpal_length{0};

	u32 const outpal_entry_mask{create_bitmask32(entry_datasize)};

	// calculate the number of entries in our output palette
	if(subpal_idx >= 0) {
		// if we're using a subpalette
		outpal_length = subpal_length;
	} else {
		// if we're using the full palette
		// limit the total number of entries in the final palette to 256
		outpal_length =
				(((subpal_count * subpal_length) > 256) ? 256
																								: subpal_count * subpal_length);
	}

	unsigned int out_datasize{outpal_length * (entry_datasize / 8)};
	u8 *out{new u8[out_datasize]};
	std::fill(out, out + out_datasize, 0);

	size_t bit_align_ptr{0}, byte_offset{0}, bit_offset{0};

	u8 entry_temp[entry_datasize_bytes];
	u8 *test_ptr;

	u8 *(*copyfunc)(u8 *, u8 *, u8 *);
	if(bigend_sys == to_coldef.get_is_big_endian()) {
		copyfunc = std::copy;
	} else {
		copyfunc = std::reverse_copy;
	}

	// for every color entry in the palette
	// convert that color into the coldef'ed version
	// mask away the extra bits
	// if entry size does not align to bytes, pack entries ***
	// resize to subpal data size if necessary ***
	for(u16 inpal_entry{0}; inpal_entry < outpal_length; ++inpal_entry) {
		byte_offset = bit_align_ptr / 8;
		bit_offset = bit_align_ptr % 8;

		// color data stored in lsb
		u32 this_color =
				(conv_col::to_coldef_color(to_coldef, data.at(inpal_entry)) &
				 outpal_entry_mask);

		// if bit gender does not match host system, reverse the value
		if(bigend_sys != to_coldef.get_is_big_endian()) {
			if(entry_datasize_bytes == 2) {
				// 2 bytes
				this_color = (this_color >> 8) | (this_color << 8);
			} else if(entry_datasize_bytes > 2) {
				// more than 2 bytes, use 32 bit
				this_color = ((this_color >> 24) & 0xff) |			// move byte 3 to byte 0
										 ((this_color << 8) & 0xff0000) |		// move byte 1 to byte 2
										 ((this_color >> 8) & 0xff00) |			// move byte 2 to byte 1
										 ((this_color << 24) & 0xff000000); // byte 0 to byte 3
			}
		}

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

palette get_pal_tlp(paldef const &paldef, u8 *data, s16 subpal_idx)
{
	if(data[0] != 0x54 || data[1] != 0x50 || data[2] != 0x4c)
		std::cerr << "Warning: Does not appear to be a valid TLP palette"
							<< std::endl;

	if(data[3] != 0) {
		throw std::invalid_argument(
				"Unsupported TPL file (only RGB mode supported)");
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
	auto out{palette()};
	out.reserve(pal_length);

	for(size_t palEntryIter = 0; palEntryIter < pal_length; ++palEntryIter) {
		out.push_back(color(data[0], data[1], data[2]));
		data += 3;
	}
	return out;
};

} // namespace conv_pal
} // namespace chrgfx