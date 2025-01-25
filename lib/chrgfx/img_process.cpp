#include "img_process.hpp"
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
	cerr << "TILE RENDERING REPORT:\n";
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

image render_palette(paldef const & paldef, coldef const & coldef, byte_t const * in_palette)
{
	uint const swatch_size {32};
	palette workpal;
	size_t pal_size {paldef.datasize_bytes()};

	decode_pal(&paldef, &coldef, in_palette, &workpal);
	size_t const image_width {swatch_size * paldef.length()}, image_height {swatch_size};

	image out_image(image_width, image_height);
	size_t tile_datasize {(size_t) (swatch_size * swatch_size)}, tile_count {paldef.length()};

	auto pixbuf = out_image.pixel_map();
	// fill one line...
	for (auto iter_color_index {0}; iter_color_index < paldef.length(); ++iter_color_index)
		fill(pixbuf + (swatch_size * iter_color_index),
			pixbuf + (swatch_size * iter_color_index) + swatch_size,
			iter_color_index);
	// duplicate that line
	for (auto iter_pixel_row {1}; iter_pixel_row < swatch_size; ++iter_pixel_row)
	{
		copy(pixbuf, pixbuf + paldef.length() * swatch_size, pixbuf + (iter_pixel_row * paldef.length() * swatch_size));
	}
	out_image.color_map(workpal);

	return out_image;
}

motoi::image<rgb_color> render_palette_full(
	paldef const & paldef, coldef const & coldef, byte_t const * in_palette, size_t const in_palette_datasize)
{
	if (in_palette_datasize < paldef.entry_datasize_bytes())
		throw runtime_error("not enough data to render a palette");

	// TOOD should we make swatch_size configurable? Is it that important...?
	uint const swatch_size {32};
	// decode each palette line into a list of basic palettes
	vector<palette> palette_lines;

	size_t const subpal_count = in_palette_datasize / paldef.datasize_bytes();

	size_t const image_width {swatch_size * paldef.length()}, image_height {swatch_size * subpal_count};
	size_t outimg_row_idx {0};

	byte_t const * ptr_subpal {in_palette};

	motoi::image<rgb_color> out_image(image_width, image_height);

	// vector<png::rgb_pixel> pixel_row(image_width);
	// png::pixel_buffer<png::rgb_pixel> pixbuf(image_width, image_height);
	palette workpal;

	for (size_t subpal_idx {0}; subpal_idx < subpal_count; ++subpal_idx)
	{
		size_t pal_size {paldef.datasize_bytes()};
		auto palbuffer {unique_ptr<byte_t>(new byte_t[pal_size])};
		decode_pal(&paldef, &coldef, ptr_subpal, &workpal);

		auto pixbuf = out_image.pixel_map() + (subpal_idx * paldef.length() * swatch_size * swatch_size);
		// fill one line...
		for (auto iter_color_index {0}; iter_color_index < paldef.length(); ++iter_color_index)
		{
			auto color = workpal[iter_color_index];
			fill(pixbuf + (swatch_size * iter_color_index), pixbuf + (swatch_size * iter_color_index) + swatch_size, color);
		}
		// duplicate that line
		for (auto iter_pixel_row {1}; iter_pixel_row < swatch_size; ++iter_pixel_row)
		{
			copy(pixbuf, pixbuf + paldef.length() * swatch_size, pixbuf + (iter_pixel_row * paldef.length() * swatch_size));
		}

		ptr_subpal += paldef.datasize_bytes();
	}

	return out_image;
}

void make_tileset(chrdef const & chrdef, image const & in_bitmap, byte_t * out_chrset)
{
	// class to chunk a bitmap into tiles
	// psuedo:
	// - get dimensions, divide by tile width
	// - width in chrs, height in chrs
	// - chrw * chrh = total tile count, provision vector<chr>
	// - src image pixel row counter = 0
	// - for each chr row
	// -- for each pixel row (tile height pixel rows)
	// --- curr pixel row = get_row(src image pixel row counter ++)
	// --- for each chr column
	// ---- read curr pixel row 8 pixels, store into output vector of chrs
	auto const chr_width {chrdef.width()}, chr_height {chrdef.height()}, bmp_width {in_bitmap.width()},
		bmp_height {in_bitmap.height()};

	if (chr_width == 0 || chr_height == 0)
		throw invalid_argument("Invalid tile dimensions");

	if (bmp_width < chr_width || bmp_height < chr_height)
		throw invalid_argument("Source image too small to form a tile");

	size_t const chr_datasize {chr_width * chr_height},
		// input image dimensions (in tiles)
		img_chrwidth {bmp_width / chr_width}, img_chrheight {bmp_height / chr_height},
		chrrow_datasize {chr_datasize * img_chrwidth},
		// offset to start of the pixel row in the next chr
		// from the end of the previous
		next_chr {chr_datasize - chr_width};

	// iters and counters
	size_t i_chrrow {0}, i_chr_pxlrow {0}, i_chrcol {0};

	// input ptrs
	byte_t const
		// pointer to start of current pixel row
		//*ptr_in_pxlrow {in_bitmap.pixbuf()},
		// pointer to current pixel
		* ptr_in_pxl {in_bitmap.pixel_map()};

	// output ptrs
	byte_t
		// pointer to start of current tile row
		*ptr_out_chrrow {out_chrset},
		// pointer to start of the current pixel row within the current tile row
		*ptr_out_pxlrow {ptr_out_chrrow},
		// pointer to current pixel
		*ptr_out_pxl {ptr_out_pxlrow};

	// for each tile row...
	for (i_chrrow = 0; i_chrrow < img_chrheight; ++i_chrrow)
	{
		// for each pixel row within the tile row...
		for (i_chr_pxlrow = 0; i_chr_pxlrow < chr_height; ++i_chr_pxlrow)
		{
			// for each tile within the pixel row...
			for (i_chrcol = 0; i_chrcol < img_chrwidth; ++i_chrcol)
			{
				// for each [tile-width] pixel...
				for (auto i {0}; i < chr_width; ++i)
					*ptr_out_pxl++ = *ptr_in_pxl++;
				ptr_out_pxl += next_chr;
			}
			ptr_out_pxl = ptr_out_pxlrow += chr_width;
		}
		ptr_out_pxl = ptr_out_pxlrow = ptr_out_chrrow += chrrow_datasize;
	}
}

} // namespace chrgfx
