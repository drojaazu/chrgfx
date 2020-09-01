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
std::map<string const, palconv_from_t> const converters_from{
		{"default", palconv_from}, {"tilelayerpro", palconv_tilelayerpro_to}};

std::map<string const, palconv_to_t> const converters_to{
		{"default", palconv_to}};

png::palette
palconv_from(paldef const &from_paldef, coldef const &from_coldef,
						 u8 const *data,
						 std::optional<unsigned int const> const &subpal_idx,
						 std::optional<conv_color::colconv_from_t> const &color_conv)
{

	conv_color::colconv_from_t convert_color{
			color_conv.value_or(conv_color::colconv_from)};

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

u8 *palconv_to(paldef const &to_paldef, coldef const &to_coldef,
							 png::palette const &data,
							 std::optional<unsigned int const> const &subpal_idx,
							 std::optional<conv_color::colconv_to_t> const &color_conv)
{
	conv_color::colconv_to_t convert_color{
			color_conv.value_or(conv_color::colconv_to)};

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
			// the first entry to read from
			start_entry{0},
			// the last entry to read from
			end_entry{0},
			// the number of entries in the final palette
			outpal_length{0};

	u32 const outpal_entry_mask{create_bitmask32(entry_datasize)};

	// calculate the ranges we'll be working with
	if(subpal_idx) {
		// if we're using a subpalette
		if(subpal_idx.value() >= subpal_count) {
			throw std::invalid_argument("Specified subpalette index is invalid");
		}
		outpal_length = subpal_length;
		start_entry = subpal_idx.value() * subpal_length;
		end_entry = start_entry + subpal_length;

	} else {
		// if we're using the full palette
		// limit the total number of entries in the final palette to 256
		outpal_length = (((to_paldef.get_palette_length()) > 256)
												 ? 256
												 : subpal_count * subpal_length);
		end_entry = outpal_length;
	}

	unsigned int out_datasize{outpal_length * entry_datasize_bytes};
	u8 *out{new u8[out_datasize]};
	std::fill(out, out + out_datasize, 0);

	size_t bit_align_ptr{0}, byte_offset{0}, bit_offset{0};

	u8 entry_temp[entry_datasize_bytes];

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
	for(unsigned int this_inpal_entry{start_entry}; this_inpal_entry < end_entry;
			++this_inpal_entry) {
		byte_offset = bit_align_ptr / 8;
		bit_offset = bit_align_ptr % 8;

		// color data stored in lsb
		u32 this_color = (convert_color(to_coldef, data.at(this_inpal_entry)) &
											outpal_entry_mask);

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

png::palette palconv_tilelayerpro_to(
		paldef const &from_paldef, coldef const &from_coldef, u8 const *data,
		std::optional<unsigned int const> const &subpal_idx,
		std::optional<conv_color::colconv_from_t> const &color_conv)
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
	if(pal_length != 16 | pal_length != 32 | pal_length != 64 |
		 pal_length != 128 | pal_length != 256) {
		throw std::invalid_argument("Invalid TPL palette length");
	}

	// start at the actual data
	data += 4;
	if(subpal_idx) {
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