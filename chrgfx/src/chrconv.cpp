#include "chrconv.hpp"

namespace chrgfx
{

byte_t * encode_chr(chrdef const & chrdef, byte_t const * basic_chr,
										byte_t * out)
{
	if(out == nullptr)
	{
		out = new byte_t[chrdef.datasize() >> 3];
		// clear out the new block
		auto p = out;
		for(auto i = 0; i < chrdef.datasize() >> 3; ++i)
			*p++ = 0;
	}

	/*
		-for every line...
		--for every pixel...
		---work pixel
		bit position in output data: yoffset[curline] + xoffset[curpixel] +
		poffset[curr]
		--- cache yoffset[curline] + xoffset[curpixel] value
		---for every bit plane...
		---extract bit from input data
		---if bit is zero, no need to do anything
		---output data |= (input_bit << bit position mod)
	*/
	ushort
			// tile dimensions
			chr_height { chrdef.height() },
			chr_width { chrdef.width() }, chr_bitdepth { chrdef.bitdepth() },
			// iterators
			// for each row of pixels in the input tile
			i_row { 0 },
			// for each pixel in that row
			i_pxl { 0 },
			// for each bitplane in that pixel
			i_bitplane { 0 },
			// bit offsets in the input tile data
			bitpos_x { 0 }, bitpos_y { 0 }, bitpos { 0 };

	ushort const
			// pointers to bit offset definitions
			*ptr_row_offset { chrdef.row_offsets().data() },
			*ptr_pxl_offset { chrdef.pixel_offsets().data() },
			*ptr_plane_offset { chrdef.plane_offsets().data() };

	byte_t const * ptr_in_pxl = basic_chr;
	byte_t this_pxl { 0 };

	// for every row of pixels...
	for(i_row = 0; i_row < chr_height; ++i_row)
	{
		bitpos_y = *ptr_row_offset++;

		// for every pixel in that row...
		for(i_pxl = 0; i_pxl < chr_width; ++i_pxl, ++ptr_pxl_offset)
		{
			this_pxl = *ptr_in_pxl++;

			// if all the bitplanes are unset (i.e. the value is zero)
			// we can skip the bitplane shenanigans altogether
			if(this_pxl == 0)
				continue;

			bitpos_x = *ptr_pxl_offset;
			// for every bit plane in that pixel...
			for(i_bitplane = 0; i_bitplane < chr_bitdepth;
					++i_bitplane, this_pxl >>= 1, ++ptr_plane_offset)
			{
				// if the bit is unset, then do not set the equivalent bit in the
				// output
				if((this_pxl & 1) == 0)
					continue;

				// get the position in the output data for this bit
				bitpos = bitpos_y + bitpos_x + *ptr_plane_offset;
				*(out + (bitpos >> 3)) |= (0x80 >> (bitpos % 8));
			}
			ptr_plane_offset = chrdef.plane_offsets().data();
		}
		ptr_pxl_offset = chrdef.pixel_offsets().data();
	}

	return out;
}

byte_t * decode_chr(chrdef const & chrdef, byte_t const * encoded_chr,
										byte_t * out)
{
	if(out == nullptr)
		out = new byte_t[chrdef.width() * chrdef.height()];

	ushort
			// tile dimensions
			chr_height { chrdef.height() },
			chr_width { chrdef.width() }, chr_bitdepth { chrdef.bitdepth() },
			i_row { 0 }, i_pxl { 0 }, i_bitplane { 0 }, work_byte { 0 },
			work_bit { 0 }, bitpos_y { 0 }, bitpos { 0 }, work_bitpos { 0 };
	byte_t this_pxl { 0 };
	byte_t * ptr_out_pixel = out;

	ushort const
			// pointers to bit offset definitions
			*ptr_row_offset { chrdef.row_offsets().data() },
			*ptr_pxl_offset { chrdef.pixel_offsets().data() },
			*ptr_plane_offset { chrdef.plane_offsets().data() };

	// for every line...
	for(i_row = 0; i_row < chr_height; ++i_row)
	{
		bitpos_y = *ptr_row_offset++;

		// for every pixel in the line...
		for(i_pxl = 0; i_pxl < chr_width; ++i_pxl, ++ptr_pxl_offset, this_pxl = 0)
		{
			// *ptr_pxl_offset is bitpos_x
			bitpos = bitpos_y + *ptr_pxl_offset;

			// for every bit planee
			for(i_bitplane = 7; i_bitplane >= chr_bitdepth;
					--i_bitplane, ++ptr_plane_offset)
			{
				work_bitpos = bitpos + *ptr_plane_offset;

				work_byte = encoded_chr[work_bitpos >> 3];

				// if work_byte_t is 0, no bits are set, so no bits will be set in the
				// output, so let's move to the next byte
				if(work_byte == 0)
					continue;

				work_bit = work_bitpos % 8;
				this_pxl |= ((work_byte << work_bit) & 0x80) >> i_bitplane;
			}
			ptr_plane_offset = chrdef.plane_offsets().data();

			*ptr_out_pixel++ = this_pxl;
		}
		ptr_pxl_offset = chrdef.pixel_offsets().data();
	}

	return out;
}

} // namespace chrgfx
