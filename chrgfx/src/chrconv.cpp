#include "chrconv.hpp"

namespace chrgfx
{

byte_t * encode_chr (chrdef const & chrdef, byte_t const * tile, byte_t * out)
{
	if (out == nullptr)
		out = new byte_t[chrdef.datasize () / 8]();

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
		chr_height {chrdef.height ()},
		chr_width {chrdef.width ()}, chr_bitdepth {chrdef.bitdepth ()},
		// bit offsets in the input tile data
		bitpos_x, bitpos_y, bitpos;

	ushort const
		// pointers to bit offset definitions
		*ptr_row_offset {chrdef.row_offsets ().data ()},
		*ptr_pxl_offset {chrdef.pixel_offsets ().data ()}, *ptr_plane_offset {chrdef.plane_offsets ().data ()};

	byte_t const * ptr_in_pxl = tile;
	byte_t this_pxl;

	// for every row of pixels...
	for (ushort i_row {0}; i_row < chr_height; ++i_row)
	{
		bitpos_y = *ptr_row_offset++;

		// for every pixel in that row...
		for (ushort i_pxl {0}; i_pxl < chr_width; ++i_pxl, ++ptr_pxl_offset)
		{
			this_pxl = *ptr_in_pxl++;

			// if all the bitplanes are unset (i.e. the value is zero)
			// we can skip the bitplane shenanigans altogether
			if (this_pxl == 0)
				continue;

			bitpos_x = *ptr_pxl_offset;
			// for every bit plane in that pixel...
			for (ushort i_bitplane {0}; i_bitplane < chr_bitdepth; ++i_bitplane, this_pxl >>= 1, ++ptr_plane_offset)
			{
				// if the bit is unset, then do not set the equivalent bit in the
				// output
				if ((this_pxl & 1) == 0)
					continue;

				// get the position in the output data for this bit
				bitpos = bitpos_y + bitpos_x + *ptr_plane_offset;
				*(out + (bitpos >> 3)) |= (0x80 >> (bitpos % 8));
			}
			ptr_plane_offset = chrdef.plane_offsets ().data ();
		}
		ptr_pxl_offset = chrdef.pixel_offsets ().data ();
	}

	return out;
}

byte_t * decode_chr (chrdef const & chrdef, byte_t const * encoded_chr, byte_t * out)
{
	if (out == nullptr)
		out = new byte_t[chrdef.width () * chrdef.height ()]();

	ushort
		// tile dimensions
		chr_height {chrdef.height ()},
		chr_width {chrdef.width ()}, chr_bitdepth {chrdef.bitdepth ()}, work_byte, work_bit, bitpos_line, bitpos_pixel,
		bitpos_plane;
	byte_t this_pxl {0};
	byte_t * ptr_out_pixel = out;

	ushort const
		// pointers to bit offset definitions
		*ptr_row_offset {chrdef.row_offsets ().data ()},
		*ptr_pixel_offset {chrdef.pixel_offsets ().data ()}, *ptr_plane_offset {chrdef.plane_offsets ().data ()};

	// for every line...
	for (ushort i_row {0}; i_row < chr_height; ++i_row)
	{
		bitpos_line = *ptr_row_offset++;

		// for every pixel in the line...
		for (ushort i_pxl {0}; i_pxl < chr_width; ++i_pxl, ++ptr_pixel_offset, this_pxl = 0)
		{
			bitpos_pixel = bitpos_line + *ptr_pixel_offset;

			// for every bit planee
			for (ushort i_bitplane {0}; i_bitplane < chr_bitdepth; ++i_bitplane, ++ptr_plane_offset)
			{
				bitpos_plane = bitpos_pixel + *ptr_plane_offset;

				work_byte = encoded_chr[bitpos_plane >> 3];

				// if work_byte_t is 0, no bits are set, so no bits will be set in the
				// output, so let's move to the next byte
				if (work_byte == 0)
					continue;

				work_bit = bitpos_plane % 8;
				// this_pxl |= ((work_byte << work_bit) & 0x80) >> i_bitplane;
				this_pxl |= ((work_byte << work_bit) & 0x80) >> (7 - i_bitplane);
			}

			*ptr_out_pixel++ = this_pxl;
			ptr_plane_offset = chrdef.plane_offsets ().data ();
		}

		ptr_pixel_offset = chrdef.pixel_offsets ().data ();
	}

	return out;
}

} // namespace chrgfx
