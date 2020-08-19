/********************************
 * pngchunk
 * Converts a bitmap into an array of tiles
 ********************************/
#include "pngchunk.hpp"

using namespace png;

namespace chrgfx
{
// class to chunk an input png into 8x8 chrs
// psuedo:
// - get dimensions, divide by 8
// - width in chrs, height in chrs
// - chrw * chrh = total tile count, provision vector<chr>
// - src image pixel row counter = 0
// - for each chr row
// -- provision temp array of chr for number of columns
// -- for each pixel row (8 pixel rows)
// --- curr pixel row = get_row(src image pixel row counter ++)
// --- for each chr column
// ---- read curr pixel row 8 pixels, store into temp array for each tile
// -- dump temp array into total chr vector

chrbank pngchunk(chrdef const &chrdef,
								 png::pixel_buffer<png::index_pixel> &bitmap)
{
	// store the chr dimensions locally
	u16 chrw{(chrdef.get_width())}, chrh{chrdef.get_height()};

	// get image dimensions in tile size & final tile count
	u32 const chr_cols = bitmap.get_width() / chrw;
	u32 const chr_rows = bitmap.get_height() / chrh;
	u32 const chr_count = chr_cols * chr_rows;

	// set new bitmap dimensions (pixels)
	// is resizing necessary? probably not
	// u32 bmph = chr_cols * chrh;
	// u32 bmpw = chr_rows * chrw;
	// bitmap->resize(bmpw, bmph);

	chrbank outbank{chrdef};

	// temp vector to hold the chrs for the current row
	std::vector<u8 *> this_chrrow;
	this_chrrow.reserve(chr_cols);
	// temp vector holding the pixels from the current bmp row
	auto this_bmprow = std::vector<index_pixel>();

	// iters and counters
	size_t bmprow_idx = 0, // tracks the current pixel row in the source bitmap
			chrrow_iter, pxlrow_iter, chrcolpxl_iter, chrcol_iter;

	// for each chr row
	for(chrrow_iter = 0; chrrow_iter < chr_rows; chrrow_iter++) {
		for(int temp = 0; temp < chr_cols; ++temp)
			this_chrrow.push_back(new u8[chrw * chrh]);
		// for each pxl row in the current chr row
		for(pxlrow_iter = 0; pxlrow_iter < chrh; pxlrow_iter++) {
			this_bmprow = bitmap.get_row(bmprow_idx++);
			// for every (chr width) pixels in the pixel row
			// chrcol_iter = 0;
			for(chrcolpxl_iter = 0; chrcolpxl_iter < chr_cols; chrcolpxl_iter++) {
				std::copy_n(this_bmprow.begin() + (chrcolpxl_iter * chrw), chrw,
										this_chrrow.at(chrcolpxl_iter) + (pxlrow_iter * chrw));
				// chrcol_iter++;
			}
		}
		// dump temp vector into outbank
		// auto t = outbank->data();
		// std::copy_n(this_chrrow.begin(), this_chrrow.end(),
		// std::back_inserter(t));
		for(size_t t = 0; t < this_chrrow.size(); t++) {
			outbank.push_back(this_chrrow.at(t));
		}
		this_chrrow.clear();
	}
	// delete[] this_chrrow;

	return outbank;
}

} // namespace chrgfx