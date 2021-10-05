#include "png_chunk.hpp"

namespace chrgfx
{
	vector<buffer> png_chunk(chrdef const & chrdef,
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

		// store the chr dimensions locally
		uint chr_pxlwidth { (chrdef.width()) }, chr_pxlheight { chrdef.height() };

		// get image dimensions in tile size & final tile count
		uint const img_row_chr { bitmap.get_width() / chr_pxlwidth };
		uint const img_col_chr { bitmap.get_height() / chr_pxlheight };
		uint const chr_count { img_row_chr * img_col_chr };

		vector<buffer> out;
		out.reserve(chr_count);
		// std::fill(outbank.begin(), outbank.end(), uptr<u8>(new u8[chrw * chrh]));
		for(size_t make_outchr_iter { 0 }; make_outchr_iter < chr_count;
				++make_outchr_iter)
			out.push_back(buffer(chr_pxlwidth * chr_pxlheight));

		vector<buffer>::iterator out_chr_iter { out.begin() };
		vector<buffer>::iterator out_chr_iter_cache { out.begin() };

		// temp vector holding the pixels from the current bmp row
		auto this_bmprow = std::vector<index_pixel>();

		// iters and counters
		size_t img_col_pxl {
			0
		}, // tracks the current pixel row in the source bitmap
				chrrow_iter { 0 }, pxlrow_iter { 0 }, col_chr_iter { 0 },
				chrcol_iter { 0 };

		// for each chr row
		for(chrrow_iter = 0; chrrow_iter < img_col_chr; ++chrrow_iter)
		{
			// point the out iter to the beginning of the current chr row
			out_chr_iter_cache = out.begin();
			out_chr_iter_cache += (chrrow_iter * img_row_chr);

			// for each pxl row in the current chr row
			for(pxlrow_iter = 0; pxlrow_iter < chr_pxlheight; ++pxlrow_iter)
			{
				// reset the out iter to the beginning of the row
				out_chr_iter = out_chr_iter_cache;
				// point to the next pixel row in the source image
				auto this_bmprow_ptr { bitmap.get_row(img_col_pxl++).begin() };

				// for every (chr width) pixels in the pixel row
				for(col_chr_iter = 0; col_chr_iter < img_row_chr; ++col_chr_iter)
				{
					std::copy_n(this_bmprow_ptr + (col_chr_iter * chr_pxlwidth),
											chr_pxlwidth,
											out_chr_iter->data() + (pxlrow_iter * chr_pxlwidth));
					out_chr_iter++;
				}
			}
		}

		return out;
	}
} // namespace chrgfx