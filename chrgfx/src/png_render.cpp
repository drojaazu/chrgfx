#include "png_render.hpp"
#include <algorithm>

using namespace std;

namespace chrgfx
{

/*
pixel_buffer<index_pixel> render(chrdef const & chrdef,
																 buffer<byte_t> const & chrdata,
																 render_traits const & rtraits)
{
	if(chrdata.size() < 1)
		throw std::length_error("Tile data buffer is empty, nothing to render");

	uint const
			// chr dimensions
			chr_pxlwidth { chrdef.width() },
			chr_pxlheight { chrdef.height() },

			chr_datasize { (chrdef.datasize() / (size_t)8) },
			chr_count { chrdata.size() / chr_datasize },

			// number of tiles in the final row that do not add up to a full row
			chrcol_modulo { (chr_count % rtraits.columns) },

			// final image dimensions (in chrs)
			outimg_chrwidth { rtraits.columns },
			outimg_chrheight { (uint)(chr_count / outimg_chrwidth +
																(chrcol_modulo > 0 ? 1 : 0)) },

			// factor in extra pixels from border, if present
			border_pixel_width { rtraits.draw_border ? outimg_chrwidth - 1 : 0 },
			border_pixel_height { rtraits.draw_border ? outimg_chrheight - 1 : 0 },

			// final image dimensions (in pixels)
			outimg_pxlwidth { (outimg_chrwidth * chr_pxlwidth) + border_pixel_width },
			outimg_pxlheight { (outimg_chrheight * chr_pxlheight) +
												 border_pixel_height };

	// pixel buffer to be sent to the final image
	auto imgbuffer { pixel_buffer<index_pixel>(outimg_pxlwidth,
																						 outimg_pxlheight) };

	// declare iters and buffers and such for processing
	size_t chrrow_iter { 0 }, chr_pxlrow_iter { 0 }, chrcol_iter { 0 },
			chr_iter { 0 }, outimg_pxlrow_idx { 0 };
	auto this_outimg_pxlrow { std::vector<index_pixel>() };
	u8 * this_chroffset { nullptr };

	// number of tiles in the current chr row; this will be constant until the
	// final row if there is a tile modulo
	uint this_chrrow_colcount { outimg_chrwidth };

#ifdef DEBUG
	cerr << dec;
	cerr << "PNG RENDERING REPORT:" << endl;
	cerr << "\tUsing border: " << to_string(rtraits.draw_border) << endl;
	cerr << "\tTile count: " << (int)(chrdata.datasize() / chrdef.datasize())
			 << endl;
	cerr << "\tTile modulo: " << (int)chrcol_modulo << endl;
	cerr << "\tTile data size: " << (int)(chr_pxlwidth * chr_pxlheight) << endl;
	cerr << "\tFinal img dimensions: " << (int)outimg_pxlwidth << "x"
			 << (int)outimg_pxlheight << endl;
#endif

	// for each chr row...
	for(chrrow_iter = 0; chrrow_iter < outimg_chrheight; ++chrrow_iter)
	{
		// check for last row
		if((outimg_chrheight - chrrow_iter) == 1 && (chrcol_modulo > 0))
		{
			this_chrrow_colcount = chrcol_modulo;
		}

		// add border if present
		if(rtraits.draw_border && chrrow_iter != 0)
		{
			this_outimg_pxlrow.clear();
			this_outimg_pxlrow.assign(outimg_pxlwidth, rtraits.trns_index);
			imgbuffer.put_row(outimg_pxlrow_idx++, this_outimg_pxlrow);
		}

		// for each pixel in that chr row...
		for(chr_pxlrow_iter = 0; chr_pxlrow_iter < chr_pxlheight; ++chr_pxlrow_iter)
		{

			this_outimg_pxlrow.clear();
			this_outimg_pxlrow.reserve(outimg_pxlwidth);

			// for each chr column in the row...
			for(chrcol_iter = 0; chrcol_iter < this_chrrow_colcount; ++chrcol_iter)
			{
				// add border pixel if present
				if(rtraits.draw_border && chrcol_iter != 0)
				{
					this_outimg_pxlrow.push_back(rtraits.trns_index);
				}
				this_chroffset =
						chr_bank[chr_iter++].get() + (chr_pxlrow_iter * chr_pxlwidth);
				std::copy(this_chroffset, this_chroffset + chr_pxlwidth,
									std::back_inserter(this_outimg_pxlrow));
			}

			// reset to point back to the first tile in the tile row
			chr_iter -= this_chrrow_colcount;

			// if we don't have enough pixels for a full out image pixel row, fill it
			// with transparent pixels
			// (this should only happen on the last row if there is a tile modulo)
			if(this_outimg_pxlrow.size() < outimg_pxlwidth)
			{
				std::fill_n(std::back_inserter(this_outimg_pxlrow),
										(outimg_pxlwidth - this_outimg_pxlrow.size()),
										rtraits.trns_index);
			}
			// put rendered row at the end of the final image
			imgbuffer.put_row(outimg_pxlrow_idx++, this_outimg_pxlrow);
		}

		// move to the next row of tiles
		chr_iter += outimg_chrwidth;
	}

	return imgbuffer;
}
*/

pixel_buffer<index_pixel> render(chrdef const & chrdef,
																 buffer<byte_t> const & chrdata,
																 render_traits const & rtraits)
{
	if(chrdata.size() < 1)
		throw length_error("Tile data is empty, nothing to render");

	size_t const
			// chr pixel dimensions
			chr_pxlwidth { chrdef.width() },
			chr_pxlheight { chrdef.height() },
			chr_datasize { chr_pxlwidth * chr_pxlheight },

			// number of tiles in the final image
			chr_count { chrdata.size() / chr_datasize },

			// excess tiles in the final row
			final_chrrow_excess { chr_count % rtraits.columns },

			// final image dimensions (in chrs)
			outimg_chrwidth { rtraits.columns },
			outimg_chrheight { chr_count / outimg_chrwidth +
												 (final_chrrow_excess > 0 ? 1 : 0) },

			// factor in extra pixels from border, if present
			// border_pixel_width { rtraits.draw_border ? outimg_chrwidth - 1 : 0 },
			// border_pixel_height { rtraits.draw_border ? outimg_chrheight - 1 : 0 },

			// final image dimensions (in pixels)
			// outimg_pxlwidth { (outimg_chrwidth * chr_pxlwidth) + border_pixel_width
			// }, outimg_pxlheight { (outimg_chrheight * chr_pxlheight) +
			//									 border_pixel_height };
			outimg_pxlwidth { outimg_chrwidth * chr_pxlwidth },
			outimg_pxlheight { outimg_chrheight * chr_pxlheight };

	auto out_pxlbuffer { pixel_buffer<index_pixel>(outimg_pxlwidth,
																								 outimg_pxlheight) };

	// declare iters and buffers and such for processing

	size_t
			// the current tile row
			i_chrrow { 0 },
			// the current pixel row in the current tile
			i_chr_pxlrow { 0 },
			// the current tile within the current tile row
			i_chrcol { 0 },
			// the current pixel row in the output image buffer
			i_out_pxlrow { 0 };

	vector<index_pixel> this_outimg_pxlrow(outimg_pxlwidth, 0);
	// this_outimg_pxlrow.reserve(outimg_pxlwidth);

	// number of tiles in the current chr row; this will be constant until the
	// final row if there is a tile modulo
	size_t this_chrrow_colcount { outimg_chrwidth };

#ifdef DEBUG
	cerr << dec;
	cerr << "PNG RENDERING REPORT:" << endl;
	cerr << "\tUsing border: " << to_string(rtraits.draw_border) << endl;
	cerr << "\tTile count: " << chr_count << endl;
	cerr << "\tTile modulo: " << final_chrrow_excess << endl;
	cerr << "\tOut tile data size: " << chr_datasize << endl;
	cerr << "\tPixel dimensions: " << outimg_pxlwidth << 'x' << outimg_pxlheight
			 << endl;
	cerr << "\tTile dimensions: " << outimg_chrwidth << 'x' << outimg_chrheight
			 << endl;
#endif

	// for each chr row...
	for(i_chrrow = 0; i_chrrow < outimg_chrheight; ++i_chrrow)
	{
		// check for last row
		if((outimg_chrheight - i_chrrow) == 1 && (final_chrrow_excess > 0))
			this_chrrow_colcount = final_chrrow_excess;

		size_t chrrow_datasize = chr_datasize * outimg_chrwidth;
		size_t chrrow_offset = chrrow_datasize * i_chrrow;
		byte_t const * ptr_this_chrrow { chrdata.data() + chrrow_offset };

		// add border if present
		// if(rtraits.draw_border && chrrow_iter != 0)
		//{
		//	this_outimg_pxlrow.clear();
		//	this_outimg_pxlrow.assign(outimg_pxlwidth, rtraits.trns_index);
		//	imgbuffer.put_row(outimg_pxlrow_idx++, this_outimg_pxlrow);
		//}

		// for each pixel in that chr row...

		for(i_chr_pxlrow = 0; i_chr_pxlrow < chr_pxlheight; ++i_chr_pxlrow)
		{
			// clear the output pixel row
			// (ensures excess columns in the final row are blank)
			auto * p = this_outimg_pxlrow.data();
			for(auto i = 0; i < outimg_pxlwidth; ++i)
				*p++ = rtraits.trns_index;

			auto * out_pxlrow = this_outimg_pxlrow.data();
			byte_t const * in_pxlrow { ptr_this_chrrow +
																 (i_chr_pxlrow * chr_pxlwidth) };

			// for each chr column in the row...
			for(i_chrcol = 0; i_chrcol < this_chrrow_colcount; ++i_chrcol)
			{
				// add border pixel if present
				// if(rtraits.draw_border && chrcol_iter != 0)
				//{
				//	this_outimg_pxlrow.push_back(rtraits.trns_index);
				//}
				// this_chroffset =
				//		(u8 *)chrdata[chr_iter++].data() + (chr_pxlrow_iter *
				// chr_pxlwidth);
				// copy(this_chroffset, this_chroffset + chr_pxlwidth,
				//					back_inserter(this_outimg_pxlrow));

				// std::copy(pixel_row, pixel_row + chr_pxlwidth,
				// back_inserter(this_outimg_pxlrow));

				std::copy(in_pxlrow, in_pxlrow + chr_pxlwidth, out_pxlrow);
				in_pxlrow += chr_datasize;
				out_pxlrow += chr_pxlwidth;
			}

			// put rendered row at the end of the final image
			out_pxlbuffer.put_row(i_out_pxlrow++, this_outimg_pxlrow);
		}
	}

	return out_pxlbuffer;
}

png::image<png::index_pixel> png_render(chrdef const & chrdef,
																				buffer<byte_t> const & chrdata,
																				palette const & pal,
																				render_traits const & rtraits)
{
	auto pixbuf { render(chrdef, chrdata, rtraits) };

	image<index_pixel> outimg(pixbuf.get_width(), pixbuf.get_height());
	outimg.set_pixbuf(pixbuf);

	outimg.set_palette(pal);

	// check for palette transparency
	if(rtraits.use_trns)
	{
		png::tRNS trans;

		// this could probably be done better...
		if(!rtraits.trns_index)
		{
			trans.push_back(0);
		}
		else
		{
			trans.resize(rtraits.trns_index + 1);
			fill(trans.begin(), trans.end(), 255);
			trans.at(rtraits.trns_index) = 0;
		}
		outimg.set_tRNS(trans);
	}

	return outimg;
}
} // namespace chrgfx
