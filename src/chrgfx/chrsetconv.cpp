#include "chrsetconv.hpp"
#include <iostream>
#ifdef DEBUG
#include <stdexcept>
#endif

using namespace std;

namespace chrgfx
{

basic_image render_chrset(
	chrdef const & chrdef, byte_t const * in_chrset, size_t const in_chrset_datasize, render_config const & render_cfg)
{
	auto const chr_width {chrdef.width()}, chr_height {chrdef.height()};
	size_t const chr_datasize {chr_width * chr_height};

	if (chr_datasize == 0)
		throw invalid_argument("Invalid tile dimension(s)");

	if (in_chrset_datasize < chr_datasize)
		throw invalid_argument("Not enough data in buffer to render a single tile");

	size_t const

		// number of chrs in the final image
		chr_count {in_chrset_datasize / chr_datasize},

		// number of excess chrs to make up the final row
		chr_excess_count {chr_count % render_cfg.row_size},

		// final image dimensions (in chrs)
		outimg_chrwidth {render_cfg.row_size},
		outimg_chrheight {chr_count / outimg_chrwidth + (chr_excess_count > 0 ? 1 : 0)},

		// data size of one full row of chrs
		// (used for pointer offsetting)
		chrrow_datasize {chr_datasize * outimg_chrwidth},

		// final image dimensions (in pixels)
		// (for processing purposes, outimg_pxlwidth is synonymous with stride)
		outimg_pxlwidth {outimg_chrwidth * chr_width}, outimg_pxlheight {outimg_chrheight * chr_height};

	basic_image out_image(outimg_pxlwidth, outimg_pxlheight);

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
		*ptr_in_chrrow {in_chrset},
		// pointer to start of the current pixel row within the current tile row
		*ptr_in_pxlrow {ptr_in_chrrow},
		// pointer to the start of the current pixel row within the current tile
		*ptr_in_chrpxlrow {ptr_in_pxlrow};

	// output data pointer
	basic_pixel * ptr_pxlrow_work {out_image.pixbuf()};

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
			// for every (chr width) pixels in the pixel row...
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

	return out_image;
}

void make_chrset(chrdef const & chrdef, basic_image const & in_bitmap, byte_t * out_chrset)
{
	// class to chunk a bitmap into tiles
	// psuedo:
	// - get dimensions, divide by tile width
	// - width in chrs, height in chrs
	// - chrw * chrh = total tile count, provision vector<chr>
	// - src image pixel row counter = 0
	// - for each chr row
	// -- for each pixel row (tile height pixel rows)
	// --- curr pixel row = get_row(src image pixel row counter ++)
	// --- for each chr column
	// ---- read curr pixel row 8 pixels, store into output vector of chrs
	auto const chr_width {chrdef.width()}, chr_height {chrdef.height()}, bmp_width {in_bitmap.width()},
		bmp_height {in_bitmap.height()};

	if (chr_width == 0 || chr_height == 0)
		throw invalid_argument("Invalid tile dimensions");

	if (bmp_width < chr_width || bmp_height < chr_height)
		throw invalid_argument("Source image too small to form a tile");

	size_t const chr_datasize {chr_width * chr_height},
		// input image dimensions (in tiles)
		img_chrwidth {bmp_width / chr_width}, img_chrheight {bmp_height / chr_height},
		chrrow_datasize {chr_datasize * img_chrwidth},
		// offset to start of the pixel row in the next chr
		// from the end of the previous
		next_chr {chr_datasize - chr_width};

	// iters and counters
	size_t i_chrrow {0}, i_chr_pxlrow {0}, i_chrcol {0};

	// input ptrs
	byte_t const
		// pointer to start of current pixel row
		//*ptr_in_pxlrow {in_bitmap.pixbuf()},
		// pointer to current pixel
		* ptr_in_pxl {in_bitmap.pixbuf()};

	// output ptrs
	byte_t
		// pointer to start of current tile row
		*ptr_out_chrrow {out_chrset},
		// pointer to start of the current pixel row within the current tile row
		*ptr_out_pxlrow {ptr_out_chrrow},
		// pointer to current pixel
		*ptr_out_pxl {ptr_out_pxlrow};

	// for each tile row...
	for (i_chrrow = 0; i_chrrow < img_chrheight; ++i_chrrow)
	{
		// for each pixel row in the tile row...
		for (i_chr_pxlrow = 0; i_chr_pxlrow < chr_height; ++i_chr_pxlrow)
		{
			// for every (chr width) pixels in the pixel row...
			for (i_chrcol = 0; i_chrcol < img_chrwidth; ++i_chrcol)
			{
				for (auto i {0}; i < chr_width; ++i)
					*ptr_out_pxl++ = *ptr_in_pxl++;
				ptr_out_pxl += next_chr;
			}
			ptr_out_pxl = ptr_out_pxlrow += chr_width;
		}
		ptr_out_pxl = ptr_out_pxlrow = ptr_out_chrrow += chrrow_datasize;
	}
}

} // namespace chrgfx
