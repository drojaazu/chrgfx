#include "png_chunk.hpp"
#include "utils.hpp"
#include <algorithm>
#include <vector>

using namespace std;
using namespace png;

namespace chrgfx
{
buffer<byte_t> png_chunk(chrdef const & chrdef,
												 pixel_buffer<index_pixel> const & bitmap)
{
	// class to chunk an input png into 8x8 chrs
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

	size_t const
			// chr pixel dimensions
			chr_pxlwidth { chrdef.width() },
			chr_pxlheight { chrdef.height() },
			chr_datasize { chr_pxlwidth * chr_pxlheight },

			// input image dimensions (in tiles)
			img_chrwidth { bitmap.get_width() / chr_pxlwidth },
			img_chrheight { bitmap.get_height() / chr_pxlheight },
			img_row_pxlwidth { bitmap.get_width() },
			chr_count { img_chrwidth * img_chrheight },

			chrrow_datasize { chr_datasize * img_chrwidth };

	buffer<byte_t> out(chr_count * chr_datasize);

	// iters and counters
	size_t i_in_pxlrow { 0 }, // tracks the current pixel row in the source bitmap
			i_chrrow { 0 }, i_chr_pxlrow { 0 }, i_chrcol { 0 }, chrcol_iter { 0 },
			// offset to start of the pixel row in the next chr from the end of the
			// previous
			next_chr { chr_datasize - chr_pxlwidth };

	byte_t
			// pointer to start of current tile row
			*ptr_out_chrrow { out.data() },
			// pointer to start of the current pixel row within the current tile row
			*ptr_out_pxlrow { ptr_out_chrrow },
			// pointer to the start of the current pixel row within the current tile
			*ptr_out_pxlchr { ptr_out_pxlrow };

	index_pixel const
			// pointer to start of the currect pixel row
			// TODO the underlying structure holding the rows in pixerl_buffer is a
			// std::vector so the data should be contiguous, shouldn't need to call
			// get_row and can just advance the pointer
			* ptr_img_pxlrow { nullptr };

	// for each tile row...
	for(i_chrrow = 0; i_chrrow < img_chrheight; ++i_chrrow)
	{

		// for each pixel row in the tile row...
		for(i_chr_pxlrow = 0; i_chr_pxlrow < chr_pxlheight; ++i_chr_pxlrow)
		{
			// point to the next pixel row in the source image
			ptr_img_pxlrow = bitmap.get_row(i_in_pxlrow++).data();

			// for every (chr width) pixels in the pixel row...
			for(i_chrcol = 0; i_chrcol < img_chrwidth; ++i_chrcol)
			{
				for(auto i = 0; i < chr_pxlwidth; ++i)
					*ptr_out_pxlchr++ = *ptr_img_pxlrow++;
				ptr_out_pxlchr += next_chr;
			}

			ptr_out_pxlchr = ptr_out_pxlrow += chr_pxlwidth;
		}

		ptr_out_pxlchr = ptr_out_pxlrow = ptr_out_chrrow += chrrow_datasize;
	}

	return out;
}
} // namespace chrgfx