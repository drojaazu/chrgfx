#ifndef CHRGFX__RENDER_H
#define CHRGFX__RENDER_H

#include "buffer.hpp"
#include "chrdef.hpp"
#include "types.hpp"
#include "utils.hpp"
#include <algorithm>
#include <png++/png.hpp>
#include <stdio.h>

using png::image;
using png::index_pixel;
using png::pixel_buffer;

namespace chrgfx
{

	/**
	 * Options for rendering a PNG image from a collection of tiles
	 */
	struct render_traits
	{
		/**
		 * Number of columns of tiles in the PNG image
		 */
		u16 cols = 8;

		/**
		 * Draws a 1 pixel border around the inner edges of tiles in the transparent
		 * entry color
		 */
		bool draw_border = false;

		/**
		 * Enable transparency
		 */
		bool use_trns = false;

		/**
		 * Palette entry to use for transparency
		 */
		u8 trns_entry = 0;
	};

	/**
	 * Renders the specified bank of tiles to a pixel buffer
	 * Does not perform data conversion
	 * Tiles should already be converted to standard format (24bit packed pixel)
	 */
	pixel_buffer<index_pixel> render(chrdef const & chrdef,
																	 buffer & chrdata,
																	 render_traits const & rtraits)
	{
		if(chrdata.size() < 1)
			throw std::length_error("Tile data is empty, nothing to render");

		ushort const
				// chr dimensions
				chr_pxlwidth { chrdef.getWidth() },
				chr_pxlheight { chrdef.getHeight() },

				// number of tiles in the final row that do not add up to a full row
				chrcol_modulo { (uint)(chrdata.size() % rtraits.cols) },

				// final image dimensions (in chrs)
				outimg_chrwidth { rtraits.cols },
				outimg_chrheight { (uint)(chrdata.size() / outimg_chrwidth +
																	(chrcol_modulo > 0 ? 1 : 0)) },

				// factor in extra pixels from border, if present
				border_pixel_width { rtraits.draw_border ? outimg_chrwidth - 1 : 0 },
				border_pixel_height { rtraits.draw_border ? outimg_chrheight - 1 : 0 },

				// final image dimensions (in pixels)
				outimg_pxlwidth { (outimg_chrwidth * chr_pxlwidth) +
													border_pixel_width },
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
		std::cerr << "PNG RENDERING REPORT:" << std::endl;
		std::cerr << "Using border: " << std::to_string(rtraits.draw_border)
							<< std::endl;
		std::cerr << "Tile count: " << (int)chr_bank.size() << std::endl;
		std::cerr << "Tile modulo: " << (int)chrcol_modulo << std::endl;
		std::cerr << "Tile data size: " << (int)(chr_pxlwidth * chr_pxlheight)
							<< std::endl;
		std::cerr << "Final img size: " << (int)outimg_pxlwidth << "x"
							<< (int)outimg_pxlheight << std::endl;
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
				this_outimg_pxlrow.assign(outimg_pxlwidth, rtraits.trns_entry);
				imgbuffer.put_row(outimg_pxlrow_idx++, this_outimg_pxlrow);
			}

			// for each pixel in that chr row...
			for(chr_pxlrow_iter = 0; chr_pxlrow_iter < chr_pxlheight;
					++chr_pxlrow_iter)
			{

				this_outimg_pxlrow.clear();
				this_outimg_pxlrow.reserve(outimg_pxlwidth);

				// for each chr column in the row...
				for(chrcol_iter = 0; chrcol_iter < this_chrrow_colcount; ++chrcol_iter)
				{
					// add border pixel if present
					if(rtraits.draw_border && chrcol_iter != 0)
					{
						this_outimg_pxlrow.push_back(rtraits.trns_entry);
					}
					this_chroffset = (u8 *)chrdata[chr_iter++].data() +
													 (chr_pxlrow_iter * chr_pxlwidth);
					std::copy(this_chroffset, this_chroffset + chr_pxlwidth,
										std::back_inserter(this_outimg_pxlrow));
				}

				// reset to point back to the first tile in the tile row
				chr_iter -= this_chrrow_colcount;

				// if we don't have enough pixels for a full out image pixel row, fill
				// it with transparent pixels (this should only happen on the last row
				// if there is a tile modulo)
				if(this_outimg_pxlrow.size() < outimg_pxlwidth)
				{
					std::fill_n(std::back_inserter(this_outimg_pxlrow),
											(outimg_pxlwidth - this_outimg_pxlrow.size()),
											rtraits.trns_entry);
				}
				// put rendered row at the end of the final image
				imgbuffer.put_row(outimg_pxlrow_idx++, this_outimg_pxlrow);
			}

			// move to the next row of tiles
			chr_iter += outimg_chrwidth;
		}

		return imgbuffer;
	}

	/**
	 * Renders the specifed bank of tiles and color palette to a PNG image
	 * Does not perform data conversion
	 * Tiles should already be converted to standard format (24bit packed pixel)
	 */
	png::image<png::index_pixel> png_render(chrdef const & chrdef,
																					vector<buffer> & chrdata,
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
			if(!rtraits.trns_entry)
			{
				trans.push_back(0);
			}
			else
			{
				trans.resize(rtraits.trns_entry + 1);
				std::fill(trans.begin(), trans.end(), 255);
				trans.at(rtraits.trns_entry) = 0;
			}
			outimg.set_tRNS(trans);
		}

		return outimg;
	}

} // namespace chrgfx

#endif