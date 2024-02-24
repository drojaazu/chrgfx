#include "imgfmt_png.hpp"

using namespace std;
using namespace motoi;

namespace chrgfx
{
blob<byte_t> make_tileset(size_t const tile_width, size_t const tile_height, image const & bitmap)
{
	// class to chunk a bitmap into 8x8 chrs
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

		chrrow_datasize {chr_datasize * img_chrwidth};

	blob<byte_t> out(chr_count * chr_datasize);

	// iters and counters
	size_t i_in_pxlrow {0}, // tracks the current pixel row in the source bitmap
		i_chrrow {0}, i_chr_pxlrow {0}, i_chrcol {0},
		// offset to start of the pixel row in the next chr from the end of the
		// previous
		next_chr {chr_datasize - tile_width};

	byte_t
		// pointer to start of current tile row
		*ptr_out_chrrow {out.data()},
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

	return out;
}

chrgfx::image make_pixbuf_from_png(png::pixel_buffer<png::index_pixel> const & png_pixbuf)
{
	auto height {png_pixbuf.get_height()}, width {png_pixbuf.get_width()};
	image out(width, height);
	byte_t * ptr = out.pixbuf();
	for (uint i_row {0}; i_row < height; ++i_row)
	{
		byte_t const * s = (byte_t const *) (png_pixbuf.get_row(i_row).data());
		copy(s, s + width, ptr);
		ptr += width;
	}
	return out;
}

chrgfx::palette make_palette_from_png(png::palette const & png_palette)
{
	chrgfx::palette out;
	size_t counter {0};
	for (auto const & color : png_palette)
		out[counter++] = chrgfx::color(color.red, color.green, color.blue);
	return out;
}
} // namespace chrgfx