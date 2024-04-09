#include "basic_gfx.hpp"

#include <utility>
#ifdef DEBUG
#include <iostream>
#endif

namespace chrgfx
{

using namespace std;
using namespace motoi;

basic_color::basic_color(uint8_t red, uint8_t green, uint8_t blue) :
		red {red},
		green {green},
		blue {blue}
{
}

string basic_color::to_html()
{
	// TODO
	return "";
}

basic_image::basic_image(uint const width, uint const height) :
		m_width {width},
		m_height {height},
		m_datasize {width * height},
		m_pixbuf {m_datasize, 0}
{
	if (m_datasize == 0)
		throw runtime_error("invalid width and/or height specified for pixmap");
}

[[nodiscard]] uint basic_image::width() const
{
	return m_width;
}

[[nodiscard]] uint basic_image::height() const
{
	return m_height;
}

[[nodiscard]] basic_pixel const * basic_image::pixbuf() const
{
	return m_pixbuf.data();
}

basic_pixel * basic_image::pixbuf()
{
	return m_pixbuf.data();
}

[[nodiscard]] basic_palette const basic_image::palette() const
{
	return m_palette;
}

basic_palette basic_image::palette()
{
	return m_palette;
}

void basic_image::palette(basic_palette const & pal)
{
	m_palette = pal;
}

basic_image render_tileset(chrgfx::chrdef const & chrdef, blob<basic_pixel> const & chrdata, render_config const & rcfg)
{
	auto const tile_width {chrdef.width()}, tile_height {chrdef.height()};
	size_t const chr_datasize {tile_width * tile_height};

	if (chr_datasize == 0)
		throw invalid_argument("Invalid tile dimension(s)");

	if (chrdata.size() < chr_datasize)
		throw invalid_argument("Not enough data in buffer to render a tile");

	size_t const

		// number of chrs in the final image
		chr_count {chrdata.size() / chr_datasize},

		// number of excess chrs to make up the final row
		chr_excess_count {chr_count % rcfg.row_size},

		// final image dimensions (in chrs)
		outimg_chrwidth {rcfg.row_size}, outimg_chrheight {chr_count / outimg_chrwidth + (chr_excess_count > 0 ? 1 : 0)},

		// data size of one full row of chrs
		// (used for pointer offsetting)
		chrrow_datasize {chr_datasize * outimg_chrwidth},

		// factor in extra pixels from border, if present
		border_pxlwidth {rcfg.draw_border ? outimg_chrwidth - 1 : 0},
		border_pxlheight {rcfg.draw_border ? outimg_chrheight - 1 : 0},

		// final image dimensions (in pixels)
		// (for processing purposes, outimg_pxlwidth is synonymous with stride)
		outimg_pxlwidth {(outimg_chrwidth * tile_width) + border_pxlwidth},
		outimg_pxlheight {(outimg_chrheight * tile_height) + border_pxlheight};

	basic_image out_buffer(outimg_pxlwidth, outimg_pxlheight);
	cerr << "addr: " << (size_t) out_buffer.pixbuf() << endl;

	// iters and cached values and such for processing
	size_t
		// number of tiles in the current chr row; this will be constant until the
		// final row if there was a chr excess
		this_chrrow_chrcount {outimg_chrwidth},
		// offset to start of the pixel row in the next chr from the end of the
		// previous
		next_chr {chr_datasize - tile_width},
		// offset to start of the next pixel row in the output
		// will be zero until the final row if there was a chr excess
		next_row {0};

	// input data pointers
	basic_pixel const
		// pointer to start of current tile row
		*ptr_in_chrrow {chrdata.data()},
		// pointer to start of the current pixel row within the current tile row
		*ptr_in_pxlrow {ptr_in_chrrow},
		// pointer to the start of the current pixel row within the current tile
		*ptr_in_chrpxlrow {ptr_in_pxlrow};

	// output data pointer
	basic_pixel * ptr_pxlrow_work {out_buffer.pixbuf()};

#ifdef DEBUG
	cerr << dec;
	cerr << "TILE RENDERING REPORT:\n";
	cerr << "\tUsing border: " << to_string(rcfg.draw_border) << '\n';
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
			// clang-format off
			next_row =
				(outimg_pxlwidth - 
					(chr_excess_count *
						(tile_width + (rcfg.draw_border ? 1 : 0))
					)
				)
				+ (rcfg.draw_border ? 1 : 0);
			// clang-format on
		}

		// add border if enabled
		if (rcfg.draw_border && iter_chrrow != 0)
			for (uint i = 0; i < outimg_pxlwidth; ++i)
				*ptr_pxlrow_work++ = rcfg.trns_index.value_or(0);

		// for each pixel row in the tile row...
		for (uint iter_chr_pxlrow = 0; iter_chr_pxlrow < tile_height; ++iter_chr_pxlrow)
		{

			// for each tile in the row...
			// copy its current pixel row to output
			for (uint iter_chrcol = 0; iter_chrcol < this_chrrow_chrcount; ++iter_chrcol)
			{
				// add border pixel if enabled
				if (rcfg.draw_border && iter_chrcol != 0)
					*ptr_pxlrow_work++ = rcfg.trns_index.value_or(0);

				for (uint i = 0; i < tile_width; ++i)
					*ptr_pxlrow_work++ = *ptr_in_chrpxlrow++;

				ptr_in_chrpxlrow += next_chr;
			}

			// next_row will be zero unless there are excess chrs in the final row
			ptr_pxlrow_work += next_row;
			ptr_in_chrpxlrow = ptr_in_pxlrow += tile_width;
		}

		ptr_in_chrpxlrow = ptr_in_pxlrow = ptr_in_chrrow += chrrow_datasize;
	}

	return out_buffer;
}

blob<byte_t> segment_tileset(chrgfx::chrdef const & chrdef, basic_image const & bitmap)
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

} // namespace chrgfx
