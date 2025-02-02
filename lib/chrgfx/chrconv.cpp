#include "chrconv.hpp"

namespace chrgfx
{
using namespace std;

void encode_chr(chrdef const & chrdef, pixel const * in_tile, byte_t * out_tile)
{
	fill_n(out_tile, chrdef.datasize_bytes(), 0);

	// clang-format off
	uint
		// bit offsets in the input tile data
		bitpos_row,
		bitpos_pixel,
		bitpos_plane,
		//iterators
		i_row,
		i_pixel,
		i_bitplane;

	uint const
		// tile dimensions
		tile_height {chrdef.height()},
		tile_width {chrdef.width()},
		tile_bpp {chrdef.bpp()},
		// pointers to bit offset definitions
		*ptr_row_offsets {chrdef.row_offsets().data()},
		*ptr_pixel_offsets {chrdef.pixel_offsets().data()},
		*ptr_plane_offsets {chrdef.plane_offsets().data()};
	// clang-format on

	pixel const * ptr_in_pixel {in_tile};
	byte_t work_byte;

	// for every row of pixels in the tile...
	for (i_row = 0; i_row < tile_height; ++i_row, ++ptr_row_offsets)
	{
		bitpos_row = *ptr_row_offsets;

		// for every pixel in that row...
		for (i_pixel = 0; i_pixel < tile_width; ++i_pixel, ++ptr_pixel_offsets, ++ptr_in_pixel)
		{
			work_byte = *ptr_in_pixel;
			// if the byte is zero, we can skip the bitplane shenanigans altogether
			// (since the output is already initialized to zero)
			if (work_byte == 0)
				continue;
			bitpos_pixel = bitpos_row + *ptr_pixel_offsets;

			// for every bit plane in that pixel...
			for (i_bitplane = 0; i_bitplane < tile_bpp; ++i_bitplane, work_byte >>= 1, ++ptr_plane_offsets)
			{
				// if the whole byte is 0, we can skip all bit checking
				if (work_byte == 0)
					break;
				// if the bit is unset, then do not set the equivalent bit in the output
				if ((work_byte & 1) == 0)
					continue;

				// get the position in the output data for this bit
				bitpos_plane = bitpos_pixel + *ptr_plane_offsets;
				*(out_tile + (bitpos_plane >> 3)) |= (0x80 >> (bitpos_plane % 8));
			}

			ptr_plane_offsets = chrdef.plane_offsets().data();
		}

		ptr_pixel_offsets = chrdef.pixel_offsets().data();
	}
}

void decode_chr(chrdef const & chrdef, byte_t const * in_tile, pixel * out_tile)
{
	// clang-format off
	uint
		// bit offsets in the input tile data
		bitpos_row,
		bitpos_pixel,
		bitpos_plane,
		//iterators
		i_row,
		i_pixel,
		i_bitplane,
		work_bit;

	uint const
		// tile dimensions
		tile_height {chrdef.height()},
		tile_width {chrdef.width()},
		tile_bpp {chrdef.bpp()},
		// pointers to bit offset definitions
		*ptr_row_offsets {chrdef.row_offsets().data()},
		*ptr_pixel_offsets {chrdef.pixel_offsets().data()},
		*ptr_plane_offsets {chrdef.plane_offsets().data()};
	// clang-format on

	pixel * ptr_out_pixel {out_tile};
	byte_t work_byte;

	// for every row of pixels in the tile...
	for (i_row = 0; i_row < tile_height; ++i_row, ++ptr_row_offsets)
	{
		bitpos_row = *ptr_row_offsets;

		// for every pixel in the row...
		for (i_pixel = 0; i_pixel < tile_width; ++i_pixel, ++ptr_pixel_offsets, ++ptr_out_pixel)
		{
			*ptr_out_pixel = 0;
			bitpos_pixel = bitpos_row + *ptr_pixel_offsets;

			// for every bit plane in that pixel...
			for (i_bitplane = 0; i_bitplane < tile_bpp; ++i_bitplane, ++ptr_plane_offsets)
			{
				bitpos_plane = bitpos_pixel + *ptr_plane_offsets;
				work_byte = in_tile[bitpos_plane >> 3];
				// if work_byte is 0, no bits are set, so no bits will be set in the output
				if (work_byte == 0)
					continue;

				work_bit = bitpos_plane % 8;
				*ptr_out_pixel |= ((work_byte << work_bit) & 0x80) >> (7 - i_bitplane);
			}

			ptr_plane_offsets = chrdef.plane_offsets().data();
		}

		ptr_pixel_offsets = chrdef.pixel_offsets().data();
	}
}

} // namespace chrgfx
