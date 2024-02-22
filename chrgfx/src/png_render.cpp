#include "png_render.hpp"
#include <png++/index_pixel.hpp>
#include <png++/pixel_buffer.hpp>

#ifdef DEBUG
#include <iostream>
#endif

using namespace std;
using namespace motoi;

namespace chrgfx
{

pixbuf render(
	size_t const tile_width, size_t const tile_height, blob<std::byte> const & chrdata, render_config const & rcfg)
{
	size_t const chr_datasize {tile_width * tile_height};

	if (chr_datasize == 0)
		throw invalid_argument("Invalid tile dimensions");

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

	pixbuf out_buffer(outimg_pxlwidth, outimg_pxlheight);

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
	std::byte const
		// pointer to start of current tile row
		*ptr_in_chrrow {chrdata.data()},
		// pointer to start of the current pixel row within the current tile row
		*ptr_in_pxlrow {ptr_in_chrrow},
		// pointer to the start of the current pixel row within the current tile
		*ptr_in_chrpxlrow {ptr_in_pxlrow};

	// output data pointer
	byte * ptr_pxlrow_work {out_buffer.data()};

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
				*ptr_pxlrow_work++ = std::byte(rcfg.trns_index);

		// for each pixel row in the tile row...
		for (uint iter_chr_pxlrow = 0; iter_chr_pxlrow < tile_height; ++iter_chr_pxlrow)
		{

			// for each tile in the row...
			// copy its current pixel row to output
			for (uint iter_chrcol = 0; iter_chrcol < this_chrrow_chrcount; ++iter_chrcol)
			{
				// add border pixel if enabled
				if (rcfg.draw_border && iter_chrcol != 0)
					*ptr_pxlrow_work++ = std::byte(rcfg.trns_index);

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

/*
pixbuf pixbuf_render(
	size_t const tile_width, size_t const tile_height, blob<std::byte> const & chrdata, render_config const & rcfg)
{
	size_t const
		// size of a single chr in bytes
		chr_datasize {tile_width * tile_height},

		// number of tiles in the final image
		chr_count {chrdata.size() / chr_datasize},

		// excess tiles in the final row
		final_chrrow_excess {chr_count % rcfg.row_size},

		// final image dimensions (in chrs)
		outimg_chrwidth {rcfg.row_size}, outimg_chrheight {chr_count / outimg_chrwidth + (final_chrrow_excess > 0 ? 1 : 0)},

		// factor in extra pixels from border, if present
		border_pxlwidth {rcfg.draw_border ? outimg_chrwidth - 1 : 0},
		border_pxlheight {rcfg.draw_border ? outimg_chrheight - 1 : 0},

		// final image dimensions
		outimg_pxlwidth {(outimg_chrwidth * tile_width) + border_pxlwidth},
		outimg_pxlheight {(outimg_chrheight * tile_height) + border_pxlheight};

	pixbuf out(outimg_pxlwidth, outimg_pxlheight);

	auto rawdata {render(tile_width, tile_height, chrdata, rcfg)};
	std::byte const * ptr_pxlrow {rawdata.data()};

	for (uint pxlrow {0}; pxlrow < outimg_pxlheight; ++pxlrow)
	{
		vector<byte> pxlrow_work(ptr_pxlrow, ptr_pxlrow + outimg_pxlwidth);
		out.put_row(pxlrow, pxlrow_work);

		ptr_pxlrow += outimg_pxlwidth;
	}

	return out;
}
*/

/*
image<index_pixel> png_render(size_t const tile_width,
	size_t const tile_height,
	blob<std::byte> const & chrdata,
	png::palette const & pal,
	render_config const & rcfg)
{
	if (pal.size() < 256)
		throw invalid_argument("Palette must contain a full 256 entries for PNG export");

	auto pixbuf {pixbuf_render(tile_width, tile_height, chrdata, rcfg)};

	image<index_pixel> outimg(pixbuf.get_width(), pixbuf.get_height());
	outimg.set_pixbuf(pixbuf);

	outimg.set_palette(pal);

	// setup transparency
	if (rcfg.use_trns)
	{
		tRNS trans(256, 255);
		trans[rcfg.trns_index] = 0;
		outimg.set_tRNS(trans);
	}

	return outimg;
}
*/

png::image<png::index_pixel> png_render(pixbuf const & pixdata, palette const & pal, render_config const & rcfg)
{
	//
	if (pal.size() < 256)
		throw invalid_argument("Palette must contain a full 256 entries for PNG export");

	png::pixel_buffer<png::index_pixel> png_pixbuf(pixdata.width(), pixdata.height());
	for (uint i_pixel_row {0}; i_pixel_row < pixdata.height(); ++i_pixel_row)
	{
		png::index_pixel * ptr = (png::index_pixel *) (pixdata.data() + i_pixel_row * pixdata.width());
		vector<png::index_pixel> pxlrow_work(ptr, ptr + pixdata.width());
		png_pixbuf.put_row(i_pixel_row, pxlrow_work);
	}
	png::image<png::index_pixel> outimg(pixdata.width(), pixdata.height());

	outimg.set_pixbuf(png_pixbuf);

	vector<png::color> png_pal;
	for (auto const & color : pal)
	{
		png_pal.emplace_back(color.red, color.green, color.blue);
	}
	outimg.set_palette(png_pal);

	// setup transparency
	if (rcfg.use_trns)
	{
		png::tRNS trans(256, 255);
		trans[rcfg.trns_index] = 0;
		outimg.set_tRNS(trans);
	}

	return outimg;
}

} // namespace chrgfx
