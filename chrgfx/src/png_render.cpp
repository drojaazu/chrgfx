#include "png_render.hpp"

using namespace png;

namespace chrgfx
{

png::image<png::index_pixel> png_render(chrbank const &chr_bank,
																				palette const &pal,
																				render_traits const &rtraits)
{
	if(chr_bank.size() < 1)
		throw std::length_error("Tile vector is empty, nothing to render");

	u32 const
			// chr dimensions
			chr_pxlwidth{chr_bank.get_chrdef().get_width()},
			chr_pxlheight{chr_bank.get_chrdef().get_height()},

			// number of tiles in the final row that do not add up to a full row
			chrcol_modulo{(u32)(chr_bank.size() % rtraits.cols)},
			// final image dimensions (in chrs)
			outimg_colwidth{rtraits.cols},
			outimg_rowheight{(u32)(chr_bank.size() / outimg_colwidth +
														 (chrcol_modulo > 0 ? 1 : 0))},
			// final image dimensions (in pixels)
			// to do: add border calculation: height + ((border size * tile rows) +
			// border size), width + ((border size * tile cols) + border size)
			outimg_pxlwidth{(u32)(outimg_colwidth * chr_pxlwidth)},
			outimg_pxlheight{(u32)(outimg_rowheight * chr_pxlheight)};

	// pixel buffer to be sent to the final image
	auto imgbuffer{pixel_buffer<index_pixel>(outimg_pxlwidth, outimg_pxlheight)};

	// declare iters and buffers and such for processing
	size_t chrrow_iter{0}, chr_pxlrow_iter{0}, chrcol_iter{0}, chr_iter{0},
			outimg_pxlrow_idx{0};
	auto this_outimg_pxlrow{std::vector<index_pixel>()};
	u8 *this_chroffset{nullptr};

	// number of tiles in the current chr row; this will be constant until the
	// final row if there is a tile modulo
	u32 this_chrrow_colcount{outimg_colwidth};

#ifdef DEBUG
	std::cerr << "Tile count: " << (int)chr_bank.size() << std::endl;
	std::cerr << "Tile modulo: " << (int)chrcol_modulo << std::endl;
	std::cerr << "Tile data size: " << (int)(chr_pxlwidth * chr_pxlheight)
						<< std::endl;
	std::cerr << "Final img size: " << (int)outimg_pxlwidth << "x"
						<< (int)outimg_pxlheight << std::endl;
#endif

	// top of image, add border if present
	// for each chr row...
	for(chrrow_iter = 0; chrrow_iter < outimg_rowheight; ++chrrow_iter) {
		// check for last row
		if((outimg_rowheight - chrrow_iter) == 1 && (chrcol_modulo > 0)) {
			this_chrrow_colcount = chrcol_modulo;
		}
		// for each pixel in that chr row...
		for(chr_pxlrow_iter = 0; chr_pxlrow_iter < chr_pxlheight;
				++chr_pxlrow_iter) {
			this_outimg_pxlrow.clear();
			// this_outimg_pxlrow.resize(outimg_pxlwidth, 0);
			this_outimg_pxlrow.reserve(outimg_pxlwidth);

			// beginning of imgout pixel row, add border if present
			// for each chr column in the row...
			for(chrcol_iter = 0; chrcol_iter < this_chrrow_colcount; ++chrcol_iter) {
				this_chroffset =
						chr_bank[chr_iter++].get() + (chr_pxlrow_iter * chr_pxlwidth);
				std::copy(this_chroffset, this_chroffset + chr_pxlwidth,
									std::back_inserter(this_outimg_pxlrow));
				// end of tile pixels, add border if present
			}

			// reset to point back to the first tile in the tile row
			chr_iter -= this_chrrow_colcount;

			// if we don't have enough pixels for a full out image pixel row, fill it
			// with transparent pixels
			// (this should only happen on the last row if there is a tile modulo)
			if(this_outimg_pxlrow.size() < outimg_pxlwidth) {
				std::fill_n(std::back_inserter(this_outimg_pxlrow),
										(outimg_pxlwidth - this_outimg_pxlrow.size()),
										rtraits.trns_entry);
			}
			// put rendered row at the end of the final image
			imgbuffer.put_row(outimg_pxlrow_idx++, this_outimg_pxlrow);
		}
		// bottom of tile row, add border if present

		// move to the next row of tiles
		chr_iter += outimg_colwidth;
	}

	image<index_pixel> outimg(outimg_pxlwidth, outimg_pxlheight);

	outimg.set_palette(pal);

	// check for palette transparency
	if(rtraits.use_trns) {
		png::tRNS trans;

		// this could probably be done better...
		if(!rtraits.trns_entry) {
			trans.push_back(0);
		} else {
			trans.resize(rtraits.trns_entry + 1);
			std::fill(trans.begin(), trans.end(), 255);
			std::cerr << trans.size() << std::endl;
			trans.at(rtraits.trns_entry) = 0;
		}
		outimg.set_tRNS(trans);
	}

	outimg.set_pixbuf(imgbuffer);

	return outimg;
}
} // namespace chrgfx