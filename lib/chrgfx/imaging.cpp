#include "imaging.hpp"
#include "image.hpp"
#include <stdexcept>
#ifdef DEBUG
#include <iostream>
#endif

using namespace std;

namespace chrgfx
{

image render_tileset(
	chrdef const & chrdef, byte_t const * in_tileset, size_t const in_tileset_datasize, render_config const & render_cfg)
{
	auto const chr_width {chrdef.width()}, chr_height {chrdef.height()};
	size_t const chr_datasize {chr_width * chr_height};

	if (chr_datasize == 0)
		throw invalid_argument("Invalid tile dimension(s)");

	if (in_tileset_datasize < chr_datasize)
		throw invalid_argument("Not enough data in buffer to render a single tile");

	size_t const
		// number of tiles in the final image
		chr_count {in_tileset_datasize / chr_datasize},
		// number of excess chrs that make up the final row
		chr_excess_count {chr_count % render_cfg.row_size},
		// final image dimensions (in tiles)
		// we don't consider the excess tile row, if present, when calculating the
		// outimg_chrheight value so that it can be used in the first processing loop
		outimg_chrwidth {render_cfg.row_size}, outimg_chrheight {chr_count / outimg_chrwidth},
		// data size of one full row of chrs
		// (used for pointer offsetting)
		chrrow_datasize {chr_datasize * outimg_chrwidth},
		// final image dimensions (in pixels)
		// (for processing purposes, outimg_pxlwidth is synonymous with stride)
		outimg_pxlwidth {outimg_chrwidth * chr_width},
		outimg_pxlheight {(outimg_chrheight + (chr_excess_count > 0 ? 1 : 0)) * chr_height};

	image out_image(outimg_pxlwidth, outimg_pxlheight);

	// iters and cached values and such for processing
	size_t
		// number of tiles in the current chr row; this will be constant until the
		// final row if there was a chr excess
		this_chrrow_chrcount {outimg_chrwidth},
		// offset to start of the pixel row in the next chr from the end of the
		// previous
		next_chr {chr_datasize - chr_width};

	// input ptrs
	byte_t const
		// pointer to start of current tile row
		*ptr_in_chrrow {in_tileset},
		// pointer to start of the current pixel row within the current tile row
		*ptr_in_pxlrow {ptr_in_chrrow},
		// pointer to the start of the current pixel row within the current tile
		*ptr_in_chrpxlrow {ptr_in_pxlrow};

	// output ptrs
	pixel * ptr_out_pxl {out_image.pixel_map()};

#ifdef DEBUG
	cerr << dec;
	cerr << "TILESET RENDERING REPORT:\n";
	cerr << "\tTile count: " << chr_count << '\n';
	cerr << "\tFinal row excess tiles: " << chr_excess_count << '\n';
	cerr << "\tOut tile data size: " << chr_datasize << '\n';
	cerr << "\tPixel dimensions: " << outimg_pxlwidth << 'x' << outimg_pxlheight << '\n';
	cerr << "\tTile dimensions: " << outimg_chrwidth << 'x' << outimg_chrheight << '\n';
#endif

	// for each tile row...
	for (uint i_chrrow = 0; i_chrrow < outimg_chrheight; ++i_chrrow)
	{
		// for each pixel row within the tile row...
		for (auto i_chr_pxlrow = 0; i_chr_pxlrow < chr_height; ++i_chr_pxlrow)
		{
			// for each tile within the pixel row...
			for (auto i_chrcol = 0; i_chrcol < this_chrrow_chrcount; ++i_chrcol)
			{
				// for each [tile-width] pixel...
				for (auto i = 0; i < chr_width; ++i)
					*ptr_out_pxl++ = *ptr_in_chrpxlrow++;
				ptr_in_chrpxlrow += next_chr;
			}
			ptr_in_chrpxlrow = ptr_in_pxlrow += chr_width;
		}
		ptr_in_chrpxlrow = ptr_in_pxlrow = ptr_in_chrrow += chrrow_datasize;
	}

	// output the remaining row if there are excess tiles
	// TODO maybe someday: Do Not Repeat Yourself with this code and the above, somehow
	if (chr_excess_count > 0)
	{
		auto next_row = (outimg_pxlwidth - (chr_excess_count * chr_width));
		for (auto i_chr_pxlrow = 0; i_chr_pxlrow < chr_height; ++i_chr_pxlrow)
		{
			for (auto i_chrcol = 0; i_chrcol < chr_excess_count; ++i_chrcol)
			{
				for (auto i = 0; i < chr_width; ++i)
					*ptr_out_pxl++ = *ptr_in_chrpxlrow++;
				ptr_in_chrpxlrow += next_chr;
			}
			ptr_out_pxl += next_row;
			ptr_in_chrpxlrow = ptr_in_pxlrow += chr_width;
		}
	}

	return out_image;
}

// TODO: make this configurable?
static uint const swatch_size {32};

image render_palette(paldef const & paldef, coldef const & coldef, byte_t const * in_palette)
{
	auto const row_width {paldef.length() * swatch_size};
	image out_image(row_width, swatch_size);

	auto ptr_out_palstart {out_image.pixel_map()}, ptr_out_swatchpixel {ptr_out_palstart};

	palette workpal;
	decode_pal(paldef, coldef, in_palette, &workpal);

	// fill one line...
	for (auto iter_color_index {0}; iter_color_index < paldef.length(); ++iter_color_index)
	{
		fill(ptr_out_swatchpixel, ptr_out_swatchpixel + swatch_size, iter_color_index);
		ptr_out_swatchpixel += swatch_size;
	}
	// duplicate that line
	for (auto iter_pixel_row {1}; iter_pixel_row < swatch_size; ++iter_pixel_row)
	{
		copy(ptr_out_palstart, ptr_out_palstart + row_width, ptr_out_palstart + (iter_pixel_row * row_width));
	}
	out_image.set_color_map(workpal);

	return out_image;
}

motoi::image<rgb_color> render_palette_full(
	paldef const & paldef, coldef const & coldef, byte_t const * in_palette, size_t const in_palette_datasize)
{
	if (in_palette_datasize < paldef.entry_datasize_bytes())
		throw runtime_error("Not enough input data to render a singley palette");

	size_t const subpal_count {in_palette_datasize / paldef.datasize_bytes()}, row_width {swatch_size * paldef.length()};
	auto ptr_in_subpal {in_palette};
	motoi::image<rgb_color> out_image(row_width, swatch_size * subpal_count);

	palette workpal;
	auto ptr_out_palstart {out_image.pixel_map()}, ptr_out_swatchpixel {ptr_out_palstart};
	for (size_t i_subpal_idx {0}; i_subpal_idx < subpal_count; ++i_subpal_idx)
	{
		decode_pal(paldef, coldef, ptr_in_subpal, &workpal);

		// make one pixel row of color swatches...
		for (auto i_color_index {0}; i_color_index < paldef.length(); ++i_color_index)
		{
			fill(ptr_out_swatchpixel, ptr_out_swatchpixel + swatch_size, workpal[i_color_index]);
			ptr_out_swatchpixel += swatch_size;
		}
		// duplicate that line for the rest of the swatch height
		for (auto i_pixel_row {1}; i_pixel_row < swatch_size; ++i_pixel_row)
		{
			copy(ptr_out_palstart, ptr_out_palstart + row_width, ptr_out_palstart + (i_pixel_row * row_width));
		}

		ptr_in_subpal += paldef.datasize_bytes();
		ptr_out_swatchpixel = ptr_out_palstart += row_width * swatch_size;
	}

	return out_image;
}

void make_tileset(chrdef const & chrdef, image const & in_image, byte_t * out_tileset)
{
	// pseudo:
	// - get dimensions, divide by tile width
	// - width in chrs, height in chrs
	// - chrw * chrh = total tile count, provision vector<chr>
	// - src image pixel row counter = 0
	// - for each chr row
	// -- for each pixel row (tile height pixel rows)
	// --- curr pixel row = get_row(src image pixel row counter ++)
	// --- for each chr column
	// ---- read curr pixel row 8 pixels, store into output vector of chrs

	// clang-format off
	size_t const
		tile_width {chrdef.width()},
		tile_height {chrdef.height()},
		image_width {in_image.width()},
		image_height {in_image.height()};
	// clang-format on

	if (tile_width == 0 || tile_height == 0)
		throw invalid_argument("Invalid tile dimensions");

	if (image_width < tile_width || image_height < tile_height)
		throw invalid_argument("Source image too small to form a tile");

	// clang-format off
	size_t const
		tile_datasize {tile_width * tile_height},
		// input image dimensions (in tiles)
		image_tilewidth {image_width / tile_width},
		image_tileheight {image_height / tile_height},
		tilerow_datasize {tile_datasize * image_tilewidth},
		// offset to start of the same pixel row in the next tile from the end of the current tile's pixel row
		next_tile {tile_datasize - tile_width};

	// iters and counters
	size_t 
		i_tile_row {0},
		i_tile_pixelrow {0},
		i_tile_column {0};

	// input ptrs
	byte_t const
		// pointer to current pixel
		* ptr_in_pixel {in_image.pixel_map()};

	// output ptrs
	byte_t
		// pointer to start of current tile row
		*ptr_out_tilerow {out_tileset},
		// pointer to start of the current pixel row within the current tile row
		*ptr_out_pixelrow {ptr_out_tilerow},
		// pointer to current pixel
		*ptr_out_pixel {ptr_out_pixelrow};
	// clang-format on

	// for each tile row...
	for (i_tile_row = 0; i_tile_row < image_tileheight; ++i_tile_row)
	{
		// for each pixel row within the tile row...
		for (i_tile_pixelrow = 0; i_tile_pixelrow < tile_height; ++i_tile_pixelrow)
		{
			// for each tile within the pixel row...
			for (i_tile_column = 0; i_tile_column < image_tilewidth; ++i_tile_column)
			{
				// for each [tile-width] pixel...
				for (auto i {0}; i < tile_width; ++i)
					*ptr_out_pixel++ = *ptr_in_pixel++;
				ptr_out_pixel += next_tile;
			}
			ptr_out_pixel = ptr_out_pixelrow += tile_width;
		}
		ptr_out_pixel = ptr_out_pixelrow = ptr_out_tilerow += tilerow_datasize;
	}
}

} // namespace chrgfx
