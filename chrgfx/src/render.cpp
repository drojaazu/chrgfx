#include "render.hpp"

using namespace png;

namespace chrgfx
{

png::image<png::index_pixel> render(bank &chr_bank, palette &pal,
																		render_traits &rtraits)
{
	if(chr_bank.size() < 1)
		throw std::length_error("Tile vector is empty, nothing to render");

	// just a shortcut so don't have to go through chr_bank all over the place...
	// std::vector<defchr> &chrs = *chr_bank.chrs;

	u16
			// chr dimensions
			chr_pxlwidth = chr_bank.get_chrdef().get_width(),
			chr_pxlheight = chr_bank.get_chrdef().get_height(),

			// number of chrs to add to end of buffer to get an evenly divisible count
			chrs_to_pad = (chr_bank.size() % rtraits.cols > 0)
												? (rtraits.cols - (chr_bank.size() % rtraits.cols))
												: 0;
#ifdef DEBUG
	std::cerr << "chrs size: " << (int)chr_bank.size() << std::endl;
	std::cerr << "chrs to pad: " << (int)chrs_to_pad << std::endl;
	std::cerr << "chr chunk size: " << (int)(chr_pxlwidth * chr_pxlheight)
						<< std::endl;
#endif

	// pad with empty chrs if necessary
	// (it's IMPORTANT that we take care of this before calculating final image
	// sizes)
	if(chrs_to_pad > 0)
		for(u16 addchr_iter = 0; addchr_iter < chrs_to_pad; ++addchr_iter)
			chr_bank.push_back(new u8[chr_pxlwidth * chr_pxlheight]{0});

	u16
			// final image dimensions (in chrs)
			outimg_colwidth = rtraits.cols,
			outimg_rowheight = chr_bank.size() / outimg_colwidth;

	u32
			// final image dimensions (in pixels)
			// to do: add border calculation: height + ((border size * tile rows) +
			// border size), width + ((border size * tile cols) + border size)
			outimg_pxlwidth = outimg_colwidth * chr_pxlwidth,
			outimg_pxlheight = outimg_rowheight * chr_pxlheight;

#ifdef DEBUG
	std::cerr << "chrs size after padding: " << (int)chr_bank.size() << std::endl;
	std::cerr << "final img size: " << (int)outimg_pxlwidth << "x"
						<< (int)outimg_pxlheight << std::endl;
#endif

	// declare iters and buffers and such for processing
	size_t chrrow_iter, chr_pxlrow_iter, chrcol_iter, chr_iter{0},
			outimg_pxlrow_idx{0};
	auto this_outimg_pxlrow{std::vector<index_pixel>()},
			this_chr_pxlrow{std::vector<index_pixel>()};
	u8 *this_chroffset;

	// pixel buffer to be sent to the final image
	auto imgbuffer{pixel_buffer<index_pixel>(outimg_pxlwidth, outimg_pxlheight)};
	// top of image, add border if present

	// for each chr row...
	for(chrrow_iter = 0; chrrow_iter < outimg_rowheight; ++chrrow_iter) {
		// for each pixel in that chr row...
		for(chr_pxlrow_iter = 0; chr_pxlrow_iter < chr_pxlheight;
				++chr_pxlrow_iter) {
			this_outimg_pxlrow.clear();
			this_outimg_pxlrow.reserve(outimg_pxlwidth);

			// beginning of imgout pixel row, add border if present
			// for each chr column in the row...
			for(chrcol_iter = 0; chrcol_iter < outimg_colwidth; ++chrcol_iter) {
				this_chroffset =
						chr_bank.at(chr_iter) + (chr_pxlrow_iter * chr_pxlwidth);
				std::copy(this_chroffset, this_chroffset + chr_pxlwidth,
									std::back_inserter(this_outimg_pxlrow));
				++chr_iter;
				// end of tile pixels, add border if present
			}

			// reset to point back to the first tile in the tile row
			chr_iter -= outimg_colwidth;
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