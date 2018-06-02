#include "render.hpp"
#include <stdio.h>
#include <algorithm>

using namespace png;
using namespace gfx;

image<index_pixel>* render(bank* chr_bank, const palette* pal,
													 render_traits* traits)
{
	if(chr_bank->size() < 1)
		throw std::length_error("Tile vector is empty, nothing to render");

	// tile dimensions
	uint16_t tileWidth = chr_bank->at(0)->get_width(),
					 tileHeight = chr_bank->at(0)->get_height();

	// add empty tiles to end of buffer so we have an evenly divisible amount
	if(chr_bank->size() % traits->cols > 0)
		chr_bank->insert(chr_bank->end(),
										 (traits->cols - (chr_bank->size() % traits->cols)),
										 new chr(tileWidth, tileHeight));

	// final image dimensions
	uint16_t
			// ... in tiles
			img_tileCols = traits->cols,
			// img_tileRows = (tiles->size() / img_tileCols) + (tiles->size() %
			// img_tileCols > 0 ? 1 : 0);
			img_tileRows = chr_bank->size() / img_tileCols,

			// ... in pixels
			// to do: add border calculation: height + ((border size * tile rows) +
			// border size), width + ((border size * tile cols) + border size)
			outimg_width = img_tileCols * tileWidth,
			outimg_height = img_tileRows * tileHeight;

	// declare iters and buffers and such for processing
	size_t tilerowIter, pxlrowIter, colIter, tileIter = 0, imgrowIdx = 0;
	std::vector<index_pixel> thisImg_pxlRow = std::vector<index_pixel>();
	std::vector<index_pixel> thisTileRow = std::vector<index_pixel>();

	// pixel buffer to be sent to the final image
	chr imgBuffer = chr(outimg_width, outimg_height);
	// top of image, add border if present

	// tile rows
	for(tilerowIter = 0; tilerowIter < img_tileRows; tilerowIter++)
	{
		// tile pixel rows
		for(pxlrowIter = 0; pxlrowIter < tileHeight; pxlrowIter++)
		{
			// reset the outimg pixel row buffer
			thisImg_pxlRow.clear();
			thisImg_pxlRow.reserve(outimg_width);
			// beginning of imgout pixel row, add border if present

			// columns
			for(colIter = 0; colIter < img_tileCols; colIter++)
			{
				thisTileRow = chr_bank->at(tileIter++)->get_row(pxlrowIter);
				// std::copy(std::begin(thisTileRow), std::end(thisTileRow),
				// std::back_inserter(thisImg_pxlRow));
				std::copy(thisTileRow.begin(), thisTileRow.end(),
									std::back_inserter(thisImg_pxlRow));
				// end of tile pixels, add border if present
			}
			// reset to point back to the first tile in the tile row
			tileIter -= img_tileCols;
			// put rendered row at the end of the final image
			imgBuffer.put_row(imgrowIdx++, thisImg_pxlRow);
		}
		// bottom of tile row, add border if present

		// move to the next row of tiles
		tileIter += img_tileCols;
	}

	image<index_pixel>* outimg =
			new image<index_pixel>(outimg_width, outimg_height);

	// check for palette shift
	if(traits->palette_offset > 0)
	{
		auto newPal = new palette(*pal);
		newPal->erase(newPal->begin(), newPal->begin() + traits->palette_offset);
		gfx::fill_pal(newPal);
		outimg->set_palette(*newPal);
		delete newPal;
	}
	else
	{
		outimg->set_palette(*pal);
	}

	// check for palette transparency
	if(traits->use_trns)
	{
		png::tRNS* trans = new png::tRNS();
		trans->push_back(0);
		outimg->set_tRNS(*trans);
		delete trans;
	}

	outimg->set_pixbuf(imgBuffer);

	return outimg;
}