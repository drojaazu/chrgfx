#include "tilesetconv.hpp"
#include <iostream>
#ifdef DEBUG
#include <stdexcept>
#endif

using namespace std;

namespace chrgfx
{

basic_image render_chrset(
	chrdef const & chrdef, byte_t const * chrset_data, size_t chrset_datasize, render_config const & rcfg)
{
	auto const chr_width {chrdef.width()}, chr_height {chrdef.height()};
	size_t const chr_datasize {chr_width * chr_height};

	if (chr_datasize == 0)
		throw invalid_argument("Invalid tile dimension(s)");

	if (chrset_datasize < chr_datasize)
		throw invalid_argument("Not enough data in buffer to render a single tile");

	size_t const

		// number of chrs in the final image
		chr_count {chrset_datasize / chr_datasize},

		// number of excess chrs to make up the final row
		chr_excess_count {chr_count % rcfg.row_size},

		// final image dimensions (in chrs)
		outimg_chrwidth {rcfg.row_size}, outimg_chrheight {chr_count / outimg_chrwidth + (chr_excess_count > 0 ? 1 : 0)},

		// data size of one full row of chrs
		// (used for pointer offsetting)
		chrrow_datasize {chr_datasize * outimg_chrwidth},

		// final image dimensions (in pixels)
		// (for processing purposes, outimg_pxlwidth is synonymous with stride)
		outimg_pxlwidth {outimg_chrwidth * chr_width}, outimg_pxlheight {outimg_chrheight * chr_height};

	basic_image out_buffer(outimg_pxlwidth, outimg_pxlheight);

	// iters and cached values and such for processing
	size_t
		// number of tiles in the current chr row; this will be constant until the
		// final row if there was a chr excess
		this_chrrow_chrcount {outimg_chrwidth},
		// offset to start of the pixel row in the next chr from the end of the
		// previous
		next_chr {chr_datasize - chr_width},
		// offset to start of the next pixel row in the output
		// will be zero until the final row if there was a chr excess
		next_row {0};

	// input data pointers
	basic_pixel const
		// pointer to start of current tile row
		*ptr_in_chrrow {chrset_data},
		// pointer to start of the current pixel row within the current tile row
		*ptr_in_pxlrow {ptr_in_chrrow},
		// pointer to the start of the current pixel row within the current tile
		*ptr_in_chrpxlrow {ptr_in_pxlrow};

	// output data pointer
	basic_pixel * ptr_pxlrow_work {out_buffer.pixbuf()};

#ifdef DEBUG
	cerr << dec;
	cerr << "TILE RENDERING REPORT:\n";
	cerr << "\tTile count: " << chr_count << '\n';
	cerr << "\tFinal row excess tiles: " << chr_excess_count << '\n';
	cerr << "\tOut tile data size: " << chr_datasize << '\n';
	cerr << "\tPixel dimensions: " << outimg_pxlwidth << 'x' << outimg_pxlheight << '\n';
	cerr << "\tTile dimensions: " << outimg_chrwidth << 'x' << outimg_chrheight << '\n';
#endif

	// for each tile row...
	for (uint iter_chrrow = 0; iter_chrrow < outimg_chrheight; ++iter_chrrow)
	{
		// check for last row
		if (iter_chrrow == (outimg_chrheight - 1) && chr_excess_count > 0)
		{
			this_chrrow_chrcount = chr_excess_count;
			next_row = (outimg_pxlwidth - (chr_excess_count * chr_width));
		}

		// for each pixel row in the tile row...
		for (uint iter_chr_pxlrow = 0; iter_chr_pxlrow < chr_height; ++iter_chr_pxlrow)
		{
			// for each tile in the row...
			// copy its current pixel row to output
			for (uint iter_chrcol = 0; iter_chrcol < this_chrrow_chrcount; ++iter_chrcol)
			{
				for (uint i = 0; i < chr_width; ++i)
					*ptr_pxlrow_work++ = *ptr_in_chrpxlrow++;

				ptr_in_chrpxlrow += next_chr;
			}

			// next_row will be zero unless there are excess chrs in the final row
			ptr_pxlrow_work += next_row;
			ptr_in_chrpxlrow = ptr_in_pxlrow += chr_width;
		}

		ptr_in_chrpxlrow = ptr_in_pxlrow = ptr_in_chrrow += chrrow_datasize;
	}

	return out_buffer;
}

size_t make_chrset(byte_t ** out, chrdef const & chrdef, basic_image const & bitmap)
{
	// class to chunk a bitmap into tiles
	// psuedo:
	// - get dimensions, divide by 8
	// - width in chrs, height in chrs
	// - chrw * chrh = total tile count, provision vector<chr>
	// - src image pixel row counter = 0
	// - for each chr row
	// -- for each pixel row (8 pixel rows)
	// --- curr pixel row = get_row(src image pixel row counter ++)
	// --- for each chr column
	// ---- read curr pixel row 8 pixels, store into output vector of chrs
	auto const tile_width {chrdef.width()}, tile_height {chrdef.height()};

	if (tile_width == 0 || tile_height == 0)
		throw invalid_argument("Invalid tile dimensions");

	if (bitmap.width() < tile_width || bitmap.height() < tile_height)
		throw invalid_argument("Source image too small to form a tile");

	size_t const
		// chr pixel dimensions
		chr_datasize {tile_width * tile_height},
		// input image dimensions (in tiles)
		img_chrwidth {bitmap.width() / tile_width}, img_chrheight {bitmap.height() / tile_height},
		chr_count {img_chrwidth * img_chrheight},

		chrrow_datasize {chr_datasize * img_chrwidth}, out_datasize {chr_count * chr_datasize};

	*out = new byte_t[out_datasize];

	// iters and counters
	size_t i_in_pxlrow {0}, // tracks the current pixel row in the source bitmap
		i_chrrow {0}, i_chr_pxlrow {0}, i_chrcol {0},
		// offset to start of the pixel row in the next chr from the end of the
		// previous
		next_chr {chr_datasize - tile_width};

	byte_t
		// pointer to start of current tile row
		*ptr_out_chrrow {*out},
		// pointer to start of the current pixel row within the current tile row
		*ptr_out_pxlrow {ptr_out_chrrow},
		// pointer to the start of the current pixel row within the current tile
		*ptr_out_pxlchr {ptr_out_pxlrow};

	byte_t const
		// pointer to start of the currect pixel row
		// TODO the underlying structure holding the rows in pixerl_buffer is a
		// std::vector so the data should be contiguous, shouldn't need to call
		// get_row and can just advance the pointer
		* ptr_img_pxlrow {nullptr};

	// for each tile row...
	for (i_chrrow = 0; i_chrrow < img_chrheight; ++i_chrrow)
	{

		// for each pixel row in the tile row...
		for (i_chr_pxlrow = 0; i_chr_pxlrow < tile_height; ++i_chr_pxlrow)
		{
			// point to the next pixel row in the source image
			ptr_img_pxlrow = bitmap.pixbuf() + (bitmap.width() * i_in_pxlrow++);

			// for every (chr width) pixels in the pixel row...
			for (i_chrcol = 0; i_chrcol < img_chrwidth; ++i_chrcol)
			{
				// TODO use memcpy here?
				for (auto i {0}; i < tile_width; ++i)
					*ptr_out_pxlchr++ = *ptr_img_pxlrow++;
				ptr_out_pxlchr += next_chr;
			}

			ptr_out_pxlchr = ptr_out_pxlrow += tile_width;
		}

		ptr_out_pxlchr = ptr_out_pxlrow = ptr_out_chrrow += chrrow_datasize;
	}

	return out_datasize;
}

} // namespace chrgfx
