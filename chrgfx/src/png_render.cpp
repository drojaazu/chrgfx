#include "png_render.hpp"

#ifdef DEBUG
#include <iostream>
#endif

using namespace std;
using namespace png;

namespace chrgfx
{

pixel_buffer<index_pixel> render(size_t const tile_width,
	size_t const tile_height,
	buffer<byte_t> const & chrdata,
	render_config const & rcfg)
{

	if(tile_width == 0 || tile_height == 0)
		throw invalid_argument("Invalid tile dimensions");

	size_t const chr_datasize { tile_width * tile_height };

	if(chrdata.size() < (chr_datasize))
		throw invalid_argument("Not enough data in buffer to render a tile");

	size_t const
		// number of tiles in the final image
		chr_count { chrdata.size() / chr_datasize },

		// excess tiles in the final row
		final_chrrow_excess { chr_count % rcfg.row_size },

		// final image dimensions (in chrs)
		outimg_chrwidth { rcfg.row_size },
		outimg_chrheight { chr_count / outimg_chrwidth +
											 (final_chrrow_excess > 0 ? 1 : 0) },

		// data size of one full row of tiles
		// (used for pointer offsetting)
		chrrow_datasize { chr_datasize * outimg_chrwidth },

		// factor in extra pixels from border, if present
		border_pxlwidth { rcfg.draw_border ? outimg_chrwidth - 1 : 0 },
		border_pxlheight { rcfg.draw_border ? outimg_chrheight - 1 : 0 },

		// final image dimensions (in pixels)
		// outimg_pxlwidth { (outimg_chrwidth * chr_pxlwidth) + border_pixel_width
		// }, outimg_pxlheight { (outimg_chrheight * chr_pxlheight) +
		//									 border_pixel_height };
		outimg_pxlwidth { (outimg_chrwidth * tile_width) + border_pxlwidth },
		outimg_pxlheight { (outimg_chrheight * tile_height) + border_pxlheight };

	pixel_buffer<index_pixel> out_pxlbuffer(outimg_pxlwidth, outimg_pxlheight);

	// iters and cached values and such for processing
	size_t
		// the current tile row
		i_chrrow { 0 },
		// the current pixel row in the current tile
		i_chr_pxlrow { 0 },
		// the current tile within the current tile row
		i_chrcol { 0 },
		// the current pixel row in the output image buffer
		i_out_pxlrow { 0 },
		// number of tiles in the current chr row; this will be constant until the
		// final row if there was a final row tile shortage
		this_chrrow_chrcount { outimg_chrwidth },
		// offset to start of the pixel row in the next chr from the end of the
		// previous
		next_chr { chr_datasize - tile_width };

	// the pixel row which will hold the amalgamated tile pixel rows
	vector<index_pixel> pxlrow_work(outimg_pxlwidth, rcfg.trns_index);

	// input data pointers
	byte_t const
		// pointer to start of current tile row
		*ptr_in_chrrow { chrdata.data() },
		// pointer to start of the current pixel row within the current tile row
		*ptr_in_pxlrow { ptr_in_chrrow },
		// pointer to the start of the current pixel row within the current tile
		*ptr_in_pxlchr { ptr_in_pxlrow };

	index_pixel * ptr_pxlrow_work { pxlrow_work.data() };

#ifdef DEBUG
	cerr << dec;
	cerr << "PNG RENDERING REPORT:" << endl;
	cerr << "\tUsing border: " << to_string(rcfg.draw_border) << endl;
	cerr << "\tTile count: " << chr_count << endl;
	cerr << "\tFinal row excess tiles: " << final_chrrow_excess << endl;
	cerr << "\tOut tile data size: " << chr_datasize << endl;
	cerr << "\tPixel dimensions: " << outimg_pxlwidth << 'x' << outimg_pxlheight
			 << endl;
	cerr << "\tTile dimensions: " << outimg_chrwidth << 'x' << outimg_chrheight
			 << endl;
#endif

	// for each tile row...
	for(i_chrrow = 0; i_chrrow < outimg_chrheight; ++i_chrrow)
	{
		// check for last row
		if(i_chrrow == (outimg_chrheight - 1) && final_chrrow_excess > 0)
			this_chrrow_chrcount = final_chrrow_excess;

		// add border if present
		if(rcfg.draw_border && i_chrrow != 0)
			out_pxlbuffer.put_row(i_out_pxlrow++, pxlrow_work);

		// for each pixel row in the tile row...
		for(i_chr_pxlrow = 0; i_chr_pxlrow < tile_height; ++i_chr_pxlrow)
		{

			// for each tile in the row...
			// copy its current pixel row to output
			for(i_chrcol = 0; i_chrcol < this_chrrow_chrcount; ++i_chrcol)
			{
				// add border pixel if present
				if(rcfg.draw_border && i_chrcol != 0)
					*ptr_pxlrow_work++ = rcfg.trns_index;

				// not bothering with std::copy/etc when it's so simple to do it this
				// way...
				for(auto i = 0; i < tile_width; ++i)
					*ptr_pxlrow_work++ = *ptr_in_pxlchr++;

				ptr_in_pxlchr += next_chr;
			}

			// put rendered row at the end of the image buffer
			out_pxlbuffer.put_row(i_out_pxlrow++, pxlrow_work);

			// clear the work pixel row
			// (ensures excess columns in the final row are blank)
			ptr_pxlrow_work = pxlrow_work.data();
			for(auto i = 0; i < outimg_pxlwidth; ++i)
				*ptr_pxlrow_work++ = rcfg.trns_index;
			ptr_pxlrow_work -= outimg_pxlwidth;

			ptr_in_pxlchr = ptr_in_pxlrow += tile_width;
		}

		ptr_in_pxlchr = ptr_in_pxlrow = ptr_in_chrrow += chrrow_datasize;
	}

	return out_pxlbuffer;
}

image<index_pixel> png_render(size_t const tile_width,
	size_t const tile_height,
	buffer<byte_t> const & chrdata,
	png::palette const & pal,
	render_config const & rcfg)
{
	if(pal.size() < 256)
		throw invalid_argument(
			"Palette must contain a full 256 entries for PNG export");

	auto pixbuf { render(tile_width, tile_height, chrdata, rcfg) };

	image<index_pixel> outimg(pixbuf.get_width(), pixbuf.get_height());
	outimg.set_pixbuf(pixbuf);

	outimg.set_palette(pal);

	// setup transparency
	if(rcfg.use_trns)
	{
		tRNS trans(256, 255);
		trans[rcfg.trns_index] = 0;
		outimg.set_tRNS(trans);
	}

	return outimg;
}
} // namespace chrgfx
