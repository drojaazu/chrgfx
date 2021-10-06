#include "palconv.hpp"

namespace chrgfx
{
namespace converters
{

using namespace std;

buffer toFormattedRgbPal(paldef const & paldef, rgbcoldef const & coldef,
												 palette const & paldata, const ushort subpal_idx = 0)
{
	/*
		Note about palettes
		Since an input png can have at most 256 colors and since some hardware
		has way, way more than that for its system palette, we're going to make
		a judgement call about the output palette:

		-If input palette length == paldef full palette length, use the full
		palette (which means this is inherently limited to hardware with full
		palettes of 256 colors or less)
		-Otherwise, use subpalette length (check that there are	at least enough
		values for a subpal)
	*/

	uint const
			// size of a single color within a palette, in bits
			entry_datasize { paldef.getEntryDatasize() },
			// as above, in bytes
			entry_datasize_bytes { entry_datasize / 8 },
			// total size of a subpalette, in bits
			subpal_datasize { paldef.getSubpalDatasize() },
			// as above, in bytes
			subpal_datasize_bytes { subpal_datasize / 8 },
			// total number of entries in a subpalette
			subpal_length { paldef.getSubpalLength() },
			// total number of subpalettes available in the given palette
			subpal_count { paldata.size() / subpal_length };

	// if(subpal_idx >= subpal_count)
	//{
	//	throw invalid_argument("Specified subpalette index is invalid");
	//}

	// outpal_length = subpal_length;
	// u8 const * inpal_data_ptr = paldata + ((subpal_datasize * subpal_idx) / 8);

	if(subpal_count == 0)
	{
		throw invalid_argument(
				"Not enough entries within the given basic palette to create a "
				"subpalette with the given paldef");
	}

	uint paldata_offset { subpal_idx * subpal_length };

	if(subpal_idx >= subpal_count)
	{
		throw out_of_range("Requested subpalette index beyond the range of the "
											 "given basic palette");
	}

	// mask for extracting color bits from the data
	// auto const outpal_entry_mask = create_bitmask32(entry_datasize);

	// provision and initialize output palette data
	//
	// data size of the formatted output palette data is
	// the length of a subpal multiplied by the size
	//
	// unsigned int out_datasize { subpal_length *
	//														(entry_datasize * subpal_length <=
	// subpal_datasize 																 ? subpal_datasize_bytes
	// : entry_datasize_bytes) };
	// TODO the above seems unncessarily complicated since we now only work
	// in subpalettes
	// just set the output buffer size to the size of a palette, regardless of
	// the size of an entry
	auto out = buffer(subpal_datasize_bytes);
	// auto out { new u8[subpal_datasize_bytes] };
	std::fill_n(out.begin(), out.end(), 0);

	// prepare space and function for data layout depending on endianness
	char entry_temp[entry_datasize_bytes];
	char * (*copyfunc)(char *, char *, char *);
	if(bigend_sys == coldef.get_is_big_endian())
	{
		copyfunc = std::copy;
	}
	else
	{
		copyfunc = std::reverse_copy;
	}

	// converted color
	u32 this_color { 0 };

	size_t outdata_bit_ptr { 0 }, byte_offset { 0 }, bit_offset { 0 };

	// for every subpalette
	//  for every color entry in the subpal
	//   convert that color to coldef'd version
	//   copy color to output

	// for every subpalette
	// for(uint this_subpal { 0 }; this_subpal < out_subpal_count; ++this_subpal)
	//	{

	auto out_iter = out.begin();

	// for every color in the subpal...
	for(uint this_subpal_entry { paldata_offset };
			this_subpal_entry < (paldata_offset + subpal_length); ++this_subpal_entry)
	{
		byte_offset = outdata_bit_ptr / 8;
		bit_offset = outdata_bit_ptr % 8;

		// do we really need to apply a mask? the color converter takes care of
		// it..?
		// this_color = toFormattedRgbColor(coldef, paldata.at(this_subpal_entry)) &
		//								 outpal_entry_mask;
		this_color = toFormattedRgbColor(coldef, paldata.at(this_subpal_entry));

		if(entry_datasize < 8)
		{
			// color entries are less than one byte in size
			out[byte_offset] |= (this_color << bit_offset);
		}
		else
		{
			// color entries are one byte or larger
			for(size_t s { 0 }; s < entry_datasize_bytes; ++s)
			{
				entry_temp[s] = (this_color >> (s * 8)) & 0xff;
			}
			copyfunc(entry_temp, entry_temp + entry_datasize_bytes, &*out_iter);
		}

		outdata_bit_ptr += entry_datasize;
	}
	// account for subpalette size
	// (really only needed when subpal data size > data size of all entries)
	// outdata_bit_ptr += (subpal_datasize - (entry_datasize * subpal_length));
	//	}

	return out;
}

palette toBasicRefPal(paldef const & paldef, rgbcoldef const & coldef,
											buffer const & paldata, const ushort subpal_idx = 0)
{

	uint const
			// size of a single color within a palette, in bits
			entry_datasize { paldef.getEntryDatasize() },
			// as above, in bytes
			entry_datasize_bytes { entry_datasize / 8 },
			// total size of a subpalette, in bits
			subpal_datasize { paldef.getSubpalDatasize() },
			// as above, in bytes
			subpal_datasize_bytes { subpal_datasize / 8 },
			// total number of entries in a subpalette
			subpal_length { paldef.getSubpalLength() },
			// total number of subpalettes available in the given palette
			subpal_count { paldata.size() / subpal_length };

	// unsigned int outpal_length { 0 };

	// u32 const inpal_entry_mask = create_bitmask32(inpal_entry_datasize);

	// points to the start of the palette data (for use with subpalettes)
	// u8 const * inpal_data_ptr { nullptr };

	if(subpal_count == 0)
	{
		throw invalid_argument(
				"Not enough entries within the given basic palette to create a "
				"subpalette with the given paldef");
	}

	uint paldata_offset { subpal_idx * subpal_length };

	if(subpal_idx >= subpal_count)
	{
		throw out_of_range("Requested subpalette index beyond the range of the "
											 "given basic palette");
	}

	palette out;
	out.reserve(subpal_length);

	// used to copy the color entry bytes into a temp array to be cast as a
	// u32
	char * (*copyfunc)(char *, char *, char *);
	if(bigend_sys == coldef.get_is_big_endian())
	{
		copyfunc = std::copy;
	}
	else
	{
		copyfunc = std::reverse_copy;
	}

	size_t bit_align_ptr { 0 },
			temp_buff_size { (size_t)((inpal_entry_datasize / 8) +
																(inpal_entry_datasize % 8 > 0 ? 1 : 0)) },
			byte_offset { 0 }, bit_offset { 0 };

	u8 temp_buff[temp_buff_size];

	u32 this_entry { 0 };

	auto out_iter = out.begin();

	// for every color in the subpal...
	for(u16 this_subpal_entry { 0 }; this_subpal_entry < subpal_length;
			++this_subpal_entry)
	{
		byte_offset = bit_align_ptr / 8;
		bit_offset = bit_align_ptr % 8;

		// copy all data for one color entry into the temp buffer...
		copyfunc(inpal_data_ptr + byte_offset,
						 inpal_data_ptr + byte_offset + temp_buff_size, temp_buff);

		// recast that buffer (array) as a 32 bit value
		this_entry = *reinterpret_cast<u32 *>(temp_buff);
		this_entry >>= bit_offset;
		this_entry &= inpal_entry_mask;

		out.push_back(convert_color(coldef, this_entry));

		// toBasicRefColor(coldef, paldata)

		bit_align_ptr += inpal_entry_datasize;

		// for special cases where the data size of the subpalette
		// is larger than the sum of the datasize of the colors
		// position the data ptr to the next actual subpalette
		if(this_subpal_entry > 0 &&
			 this_subpal_entry % paldef.getSubpalLength() == 0)
		{
			bit_align_ptr +=
					subpal_datasize - (inpal_entry_datasize * paldef.getSubpalLength());
		}
	}

	return out;
}

png::palette
cvfrom_tilelayerpro_pal(paldef const & from_paldef,
												rgbcoldef const & from_coldef, u8 const * data,
												std::optional<unsigned int const> const & subpal_idx)
{
	if(data[0] != 0x54 || data[1] != 0x50 || data[2] != 0x4c)
		std::cerr << "Warning: Does not appear to be a valid TLP palette"
							<< std::endl;

	if(data[3] != 0)
	{
		throw std::invalid_argument(
				"Unsupported TPL file (only RGB mode supported)");
	}

	uint pal_length = from_paldef.getPalLength();

	// these are the only valid lengths
	if(pal_length != 16 & pal_length != 32 & pal_length != 64 &
		 pal_length != 128 & pal_length != 256)
	{
		throw std::invalid_argument("Invalid TPL palette length");
	}

	// start at the actual data
	data += 4;
	if(subpal_idx)
	{
		if(subpal_idx.value() > (from_paldef.getSubpalCount() - 1))
		{
			throw std::invalid_argument(
					"Subpalette index greater than subpalette count");
		}
		pal_length = from_paldef.getSubpalLength();
		data += (subpal_idx.value() * from_paldef.getSubpalLength() * 3);
	}

	if(pal_length > 256)
		pal_length = 256;

	auto out { palette() };
	out.reserve(pal_length);

	for(size_t this_pal_entry = 0; this_pal_entry < pal_length; ++this_pal_entry)
	{
		out.push_back(color(data[0], data[1], data[2]));
		data += 3;
	}
	return out;
};

} // namespace converters
} // namespace chrgfx