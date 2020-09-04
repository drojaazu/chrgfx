/********************************
 * conv_pal
 * Convert palette formats
 ********************************/
#include "conv_pal.hpp"
#include "gfxdef.hpp"

namespace chrgfx
{
namespace conv_palette
{

// converter maps
std::map<string const, cvfrom_pal_t> const converters_from{
		{"default", cvfrom_pal}, {"tilelayerpro", cvfrom_tilelayerpro_pal}};

std::map<string const, cvto_pal_t> const converters_to{{"default", cvto_pal}};

png::palette
cvfrom_pal(paldef const &from_paldef, coldef const &from_coldef, u8 const *data,
					 std::optional<unsigned int const> const &subpal_idx,
					 std::optional<conv_color::cvfrom_col_t> const &color_conv)
{

	conv_color::cvfrom_col_t convert_color{
			color_conv.value_or(conv_color::cvfrom_color)};

	uint const subpal_count{from_paldef.get_subpal_count()};

	unsigned int const
			// size of one entry in the paldef palette (in bits)
			inpal_entry_datasize{from_paldef.get_entry_datasize()},
			// number of entries in a subpalette
			subpal_length{from_paldef.get_subpal_length()},
			// total size of a paldef subpalette (in bits)
			subpal_datasize{from_paldef.get_subpal_datasize()};

	unsigned int outpal_length{0};

	u32 const inpal_entry_mask = create_bitmask32(inpal_entry_datasize);

	// points to the start of the palette data (for use with subpalettes)
	u8 const *inpal_data_ptr{nullptr};

	// calculate the number of entries in our output palette
	if(subpal_idx) {
		if(subpal_idx.value() >= subpal_count) {
			throw std::invalid_argument("Specified subpalette index is invalid");
		}

		// if we're using a subpalette
		outpal_length = subpal_length;
		inpal_data_ptr = data + ((subpal_datasize * subpal_idx.value()) / 8);

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
	for(u16 this_inpal_entry{0}; this_inpal_entry < outpal_length;
			++this_inpal_entry) {
		byte_offset = bit_align_ptr / 8;
		bit_offset = bit_align_ptr % 8;

		// copy all data for one color entry into the temp buffer...
		copyfunc(inpal_data_ptr + byte_offset,
						 inpal_data_ptr + byte_offset + temp_buff_size, temp_buff);

		// recast that buffer (array) as a 32 bit value
		this_entry = *reinterpret_cast<u32 *>(temp_buff);
		this_entry >>= bit_offset;
		this_entry &= inpal_entry_mask;

		out.push_back(convert_color(from_coldef, this_entry));

		bit_align_ptr += inpal_entry_datasize;

		// for special cases where the data size of the subpalette
		// is larger than the sum of the datasize of the colors
		// position the data ptr to the next actual subpalette
		if(this_inpal_entry > 0 &&
			 this_inpal_entry % from_paldef.get_subpal_length() == 0) {
			bit_align_ptr += subpal_datasize -
											 (inpal_entry_datasize * from_paldef.get_subpal_length());
		}
	}

	return out;
}

u8 *cvto_pal(paldef const &to_paldef, coldef const &to_coldef,
						 png::palette const &inpal,
						 std::optional<conv_color::cvto_col_t> const &color_conv)
{
	/*
		Note about palettes
		Since an input png can, at most, have 256 colors and since some hardware has
		way, way more than that for its system palette, we're going to make a
		judgement call about the output palette:

		-If input palette length == paldef full palette length, use the full palette
		(which means this is inherently limited to hardware with full palettes of
		256 colors or less)
		-Otherwise, use subpalette length (check that there are	at least enough
		values for a subpal)
	*/
	conv_color::cvto_col_t convert_color{
			color_conv.value_or(conv_color::cvto_color)};

	unsigned int const
			// total number of subpalettes in the system palette
			subpal_count{to_paldef.get_subpal_count()},
			// size of a single color within a palette, in bits
			entry_datasize{to_paldef.get_entry_datasize()},
			// as above, in bytes
			entry_datasize_bytes{entry_datasize / 8},
			// total number of entries in a single subpalette
			subpal_length{to_paldef.get_subpal_length()},
			// total size of a single subpalette, in bits
			subpal_datasize{to_paldef.get_subpal_datasize()};

	unsigned int
			// the number of entries in the final palette
			outpal_length{0},
			// the number of subpalettes we'll be outputting
			out_subpal_count{0};

	// calculate the ranges we'll be working with
	if(inpal.size() == to_paldef.get_palette_length()) {
		out_subpal_count = to_paldef.get_subpal_count();
		// palette has exact number of entries for a full output palette
		outpal_length = to_paldef.get_palette_length();
	} else {
		// consider the input palette to be subpalette sized
		if(inpal.size() < subpal_length) {
			throw std::invalid_argument(
					"PNG palette does not have enough entries for a subpalette");
		}
		out_subpal_count = 1;
		outpal_length = subpal_length;
	}

	// mask for extracting color bits from the data
	u32 const outpal_entry_mask{create_bitmask32(entry_datasize)};

	// provision and initialize output palette data
	unsigned int out_datasize{outpal_length *
														(entry_datasize * subpal_length <= subpal_datasize
																 ? subpal_datasize / 8
																 : entry_datasize_bytes)};
	u8 *out{new u8[out_datasize]};
	std::fill_n(out, out_datasize, 0);

	// prepare space and function for data layout depending on endianness
	u8 entry_temp[entry_datasize_bytes];
	u8 *(*copyfunc)(u8 *, u8 *, u8 *);
	if(bigend_sys == to_coldef.get_is_big_endian()) {
		copyfunc = std::copy;
	} else {
		copyfunc = std::reverse_copy;
	}

	// converted color
	u32 this_color{0};
	uint inpal_offset{0};
	size_t outdata_bit_ptr{0}, byte_offset{0}, bit_offset{0};

	// for every subpalette
	//  for every color entry in the subpal
	//   convert that color to coldef'd version
	//   copy color to output

	// for every subpalette
	for(uint this_subpal{0}; this_subpal < out_subpal_count; ++this_subpal) {
		inpal_offset = this_subpal * subpal_length;
		// for every color in the subpal
		for(uint this_subpal_entry{inpal_offset};
				this_subpal_entry < (inpal_offset + subpal_length);
				++this_subpal_entry) {
			byte_offset = outdata_bit_ptr / 8;
			bit_offset = outdata_bit_ptr % 8;

			this_color = convert_color(to_coldef, inpal.at(this_subpal_entry)) &
									 outpal_entry_mask;

			if(entry_datasize < 8) {
				// color entries are less than one byte in size
				out[byte_offset] |= (this_color << bit_offset);
			} else {
				// color entries are one byte or larger
				for(size_t s{0}; s < entry_datasize_bytes; ++s) {
					entry_temp[s] = (this_color >> (s * 8)) & 0xff;
				}
				copyfunc(entry_temp, entry_temp + entry_datasize_bytes,
								 out + byte_offset);
			}

			outdata_bit_ptr += entry_datasize;
		}
		// account for subpalette size
		// (really only needed when subpal data size > data size of all entries)
		outdata_bit_ptr += (subpal_datasize - (entry_datasize * subpal_length));
	}

	return out;
}

png::palette cvfrom_tilelayerpro_pal(
		paldef const &from_paldef, coldef const &from_coldef, u8 const *data,
		std::optional<unsigned int const> const &subpal_idx,
		std::optional<conv_color::cvfrom_col_t> const &color_conv)
{
	if(data[0] != 0x54 || data[1] != 0x50 || data[2] != 0x4c)
		std::cerr << "Warning: Does not appear to be a valid TLP palette"
							<< std::endl;

	if(data[3] != 0) {
		throw std::invalid_argument(
				"Unsupported TPL file (only RGB mode supported)");
	}

	uint pal_length = from_paldef.get_palette_length();

	// these are the only valid lengths
	if(pal_length != 16 & pal_length != 32 & pal_length != 64 &
		 pal_length != 128 & pal_length != 256) {
		throw std::invalid_argument("Invalid TPL palette length");
	}

	// start at the actual data
	data += 4;
	if(subpal_idx) {
		if(subpal_idx.value() > (from_paldef.get_subpal_count() - 1)) {
			throw std::invalid_argument(
					"Subpalette index greater than subpalette count");
		}
		pal_length = from_paldef.get_subpal_length();
		data += (subpal_idx.value() * from_paldef.get_subpal_length() * 3);
	}

	if(pal_length > 256)
		pal_length = 256;

	auto out{palette()};
	out.reserve(pal_length);

	for(size_t this_pal_entry = 0; this_pal_entry < pal_length;
			++this_pal_entry) {
		out.push_back(color(data[0], data[1], data[2]));
		data += 3;
	}
	return out;
};

} // namespace conv_palette
} // namespace chrgfx