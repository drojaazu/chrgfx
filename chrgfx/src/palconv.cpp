#include "palconv.hpp"
#include "colconv.hpp"
#include "utils.hpp"

namespace chrgfx
{

using namespace std;

byte_t * encode_pal (paldef const & paldef, coldef const & coldef, png::palette const & palette)
{
	size_t const
		// size of a single color within a palette, in bits
		entry_datasize {paldef.entry_datasize ()},
		// as above, in bytes
		entry_datasize_bytes {((unsigned) (entry_datasize >> 3)) + (entry_datasize % 8 > 0 ? 1 : 0)},
		// as above, in bytes
		subpal_datasize_bytes {(unsigned) (paldef.datasize () >> 3)},
		// total number of entries in a subpalette
		subpal_length {paldef.pal_length ()};
	// total number of subpalettes available in the given palette
	// subpal_count { basic_palette.size() / subpal_length };
	/*
		if(subpal_count == 0)
		{
			throw invalid_argument(
					"Not enough entries within the given basic palette to create a "
					"subpalette with the given paldef");
		}

	if(subpal_idx >= subpal_count)
	{
		throw out_of_range("Requested subpalette index beyond the range of the "
											 "given palette data");
	}
*/

	// used to copy the color entry bytes into a temp array
	// to be cast as a machine-native u32
	char * (*copyfunc) (char *, char *, char *);
	if (bigend_sys == coldef.big_endian ())
	{
		copyfunc = std::copy;
	}
	else
	{
		copyfunc = std::reverse_copy;
	}

	size_t
		// tracks bit position within the paldata
		bit_align_pos {0},
		byte_align_pos {0},
		// mod 8 of bitpos, indicating number of bits past the last byte_t aligned
		// boundary at which the read pointer is, and thus the number of bits
		// that the entry should be shifted in order to bring it to its correct
		// value
		bit_align_mod {0};

	auto out {new byte_t[subpal_datasize_bytes]};
	std::fill_n (out, subpal_datasize_bytes, 0);

	// converted color
	u32 this_entry {0};
	auto out_ptr {out};
	size_t entry_count {0};

	for (auto const & color : palette)
	{
		// iterate over the colors in the color palette, but only up to the number
		// of colors in the output format subpal length
		if (entry_count >= subpal_length)
			break;

		byte_align_pos = bit_align_pos >> 3;
		bit_align_mod = bit_align_pos % 8;

		switch (coldef.type ())
		{
			case rgb:
				this_entry = encode_col (static_cast<rgbcoldef const &> (coldef), color);
				break;
			case ref:
				this_entry = encode_col (static_cast<refcoldef const &> (coldef), color);
				break;
			default:
				// should never happen, but for completeness:
				throw runtime_error ("Invalid coldef type");
		}

		if (bit_align_mod > 0)
		{
			this_entry <<= bit_align_mod;
			this_entry |= out[byte_align_pos];
		}

		copyfunc ((char *) &this_entry, ((char *) &this_entry) + entry_datasize_bytes, (char *) out_ptr);

		out_ptr += entry_datasize_bytes;
		++entry_count;

		byte_align_pos += entry_datasize;
	}

	return out;
}

palette decode_pal (paldef const & paldef, coldef const & coldef, byte_t const * palette)
{

	// some basic data geometry
	size_t const
		// size of a single color within a palette, in bits
		entry_datasize {paldef.entry_datasize ()},
		// as above, in bytes
		entry_datasize_bytes {(entry_datasize >> 3) + (entry_datasize % 8 > 0 ? 1 : 0)},
		// total size of a subpalette, in bits
		subpal_datasize {paldef.datasize ()},
		// total number of entries in a subpalette
		subpal_length {paldef.pal_length ()};

	// used to copy the color entry bytes into a temp array
	// to be cast as a machine-native u32
	char * (*copyfunc) (char *, char *, char *);
	if (bigend_sys == coldef.big_endian ())
	{
		copyfunc = std::copy;
	}
	else
	{
		copyfunc = std::reverse_copy;
	}

	// processing loop setup
	size_t
		// tracks bit position within the paldata
		bit_align_pos {0},
		// mod 8 of bitpos, indicating number of bits past the last byte_t aligned
		// boundary at which the read pointer is, and thus the number of bits
		// that the entry should be shifted in order to bring it to its correct
		// value
		bit_align_mod {0}, byte_offset {0}, temp_buff_size {entry_datasize_bytes};

	// the temporary buffer will hold the entry value "extracted" from the palette
	// data byte by byte before it is copied to another buffer respecting the
	// endianness of the local machine
	byte_t temp_buff[temp_buff_size];
	fill_n (temp_buff, temp_buff_size, 0);

	// the entry buffer will hold the "extracted" palette entry as a native
	// integer, whereupon bit operations can be performed
	u32 entry_buff {0};

	u32 const entry_buff_bitmask = create_bitmask32 (entry_datasize);

	// auto out = new color[subpal_length];
	// png++ expects a full 8 bit palette
	png::palette out (256, color {0, 0, 0});
	// out.reserve(subpal_length);

	auto paldata_iter = palette;

	// processing loop
	// for every color in the subpal
	for (size_t this_subpal_entry {0}; this_subpal_entry < subpal_length; ++this_subpal_entry)
	{
		byte_offset = bit_align_pos / 8;
		bit_align_mod = bit_align_pos % 8;

		// this is inefficient when working with bit-sized palette entries
		// in that we are copying the same byte_t multiple times
		// (e.g. a 2 bit palette will work with the same byte_t 4 times)
		// however, trying to account for this would require additional code
		// complexity that would likely outweigh recopying a single byte
		// a few times...

		// copy all data for one color entry into the temp buffer
		copyfunc (
			(char *) paldata_iter + byte_offset, (char *) paldata_iter + byte_offset + temp_buff_size, (char *) temp_buff);

		// recast that buffer (array) as a 32 bit value, shift it into its
		// correct position, and mask off extraneous upper bits
		entry_buff = *reinterpret_cast<u32 *> (temp_buff);
		entry_buff >>= bit_align_mod;
		entry_buff &= entry_buff_bitmask;

		// out.push_back(decode_col(coldef, entry_buff));

		switch (coldef.type ())
		{
			case rgb:
				out[this_subpal_entry] = decode_col (static_cast<rgbcoldef const &> (coldef), entry_buff);
				break;
			case ref:
				out[this_subpal_entry] = decode_col (static_cast<refcoldef const &> (coldef), entry_buff);
				break;
			default:
				// should never happen, but for completeness
				throw runtime_error ("Invalid coldef type");
		}

		// advance the read position
		bit_align_pos += entry_datasize;

		// for special cases where the data size of the subpalette
		// is larger than the sum of the datasize of the colors,
		// position the data ptr to the next actual subpalette
		// TODO can this be optimized into static values outside of the loop?
		if (this_subpal_entry > 0 && this_subpal_entry % subpal_length == 0)
		{
			bit_align_pos += subpal_datasize - (entry_datasize * subpal_length);
		}

		// paldata_iter += entry_datasize_bytes;
	}

	return out;
}

} // namespace chrgfx