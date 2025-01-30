#include "chrconv.hpp"

namespace chrgfx
{
using namespace std;

void encode_chr(chrdef const & chrdef, pixel const * in_tile, byte_t * out_tile)
{
	fill_n(out_tile, chrdef.datasize_bytes(), 0);

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
	uint
		// tile dimensions
		chr_height {chrdef.height()},
		chr_width {chrdef.width()}, chr_bitdepth {chrdef.bpp()},
		// bit offsets in the input tile data
		bitpos_x, bitpos_y, bitpos;

	uint const
		// pointers to bit offset definitions
		*ptr_row_offset {chrdef.row_offsets().data()},
		*ptr_pxl_offset {chrdef.pixel_offsets().data()}, *ptr_plane_offset {chrdef.plane_offsets().data()};

	byte_t const * ptr_in_pxl = in_tile;
	byte_t this_pxl;

	// for every row of pixels...
	for (uint i_row {0}; i_row < chr_height; ++i_row)
	{
		bitpos_y = *ptr_row_offset++;

		// for every pixel in that row...
		for (uint i_pxl {0}; i_pxl < chr_width; ++i_pxl, ++ptr_pxl_offset)
		{
			this_pxl = *ptr_in_pxl++;

			// if all the bitplanes are unset (i.e. the value is zero)
			// we can skip the bitplane shenanigans altogether
			if (this_pxl == 0)
				continue;

			bitpos_x = *ptr_pxl_offset;
			// for every bit plane in that pixel...
			for (uint i_bitplane {0}; i_bitplane < chr_bitdepth; ++i_bitplane, this_pxl >>= 1, ++ptr_plane_offset)
			{
				// if the bit is unset, then do not set the equivalent bit in the
				// output
				if ((this_pxl & 1) == 0)
					continue;

				// get the position in the output data for this bit
				bitpos = bitpos_y + bitpos_x + *ptr_plane_offset;
				*(out_tile + (bitpos >> 3)) |= (0x80 >> (bitpos % 8));
			}
			ptr_plane_offset = chrdef.plane_offsets().data();
		}
		ptr_pxl_offset = chrdef.pixel_offsets().data();
	}
}

void decode_chr(chrdef const & chrdef, byte_t const * in_tile, pixel * out_tile)
{
	uint
		// tile dimensions
		chr_height {chrdef.height()},
		chr_width {chrdef.width()}, chr_bitdepth {chrdef.bpp()}, work_bit,
		// bitwise position for the current row
		bitpos_row,
		// bitwise position for the curent pixel
		bitpos_pixel,
		// bitwise position for the current plane
		bitpos_plane;

	byte_t work_byte {0}, this_pxl {0};
	byte_t * ptr_out_pixel = out_tile;

	uint const
		// pointers to bit offset definitions
		*ptr_row_offset {chrdef.row_offsets().data()},
		*ptr_pixel_offset {chrdef.pixel_offsets().data()}, *ptr_plane_offset {chrdef.plane_offsets().data()};

	// for every line...
	for (uint i_row {0}; i_row < chr_height; ++i_row)
	{
		bitpos_row = *ptr_row_offset++;

		// for every pixel in the line...
		for (uint i_pxl {0}; i_pxl < chr_width; ++i_pxl, ++ptr_pixel_offset, this_pxl = 0)
		{
			bitpos_pixel = bitpos_row + *ptr_pixel_offset;

			// for every bit plane
			for (uint i_bitplane {0}; i_bitplane < chr_bitdepth; ++i_bitplane)
			{
				bitpos_plane = bitpos_pixel + *ptr_plane_offset++;

				// reminder: >> 3 is to divide by 8 (since we're getting a byte offset using bits)
				work_byte = in_tile[bitpos_plane >> 3];

				// if work_byte is 0, no bits are set, so no bits will be set in the
				// output, so let's move to the next byte_t
				if (work_byte == 0)
					continue;

				work_bit = bitpos_plane % 8;
				// this_pxl |= ((work_byte << work_bit) & 0x80) >> i_bitplane;
				// TODO: i_bitplane is only used here, can we work this '7 - ' math  into the loop header?
				this_pxl |= ((work_byte << work_bit) & 0x80) >> (7 - i_bitplane);
			}

			*ptr_out_pixel++ = this_pxl;
			ptr_plane_offset = chrdef.plane_offsets().data();
		}

		ptr_pixel_offset = chrdef.pixel_offsets().data();
	}
}

} // namespace chrgfx
