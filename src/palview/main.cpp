#include "blob.hpp"
#include "fstreams.hpp"
#include "imgfmt_png.hpp"
#include "setup.hpp"
#include <chrgfx/chrgfx.hpp>

#include <iostream>
#include <png++/rgb_pixel.hpp>

using namespace std;
using namespace chrgfx;
using namespace motoi;

uint const swatch_size {32};

chrgfx::coldef const * work_coldef {&chrgfx::gfxdefs::col_bgr_333_packed};
chrgfx::paldef const * work_paldef {&chrgfx::gfxdefs::pal_16bit_256color};

void render_palette_line()
{
	basic_palette workpal;
	// load palette, check validity of palette line
	{

		ifstream paldata {ifstream_checked(cfg.paldata_name)};
		size_t pal_size {work_paldef->datasize() / 8};
		byte_t palbuffer[pal_size];

		paldata.seekg(cfg.pal_line * pal_size, ios::beg);
		paldata.read(reinterpret_cast<char *>(palbuffer), pal_size);
		if (paldata.gcount() != pal_size)
			throw runtime_error("Could not read enough data for a complete color palette");

		decode_pal(*work_paldef, *work_coldef, palbuffer, &workpal);
	}

	// generate swatch tiles
	size_t tile_datasize {(size_t) (swatch_size * swatch_size)}, tile_count {work_paldef->length()};
	blob tile_buffer(tile_datasize * tile_count);
	uchar * tile_ptr {reinterpret_cast<uchar *>(tile_buffer.data())};
	for (uchar iter_tile {0}; iter_tile < tile_count; ++iter_tile)
	{
		fill(tile_ptr, tile_ptr + tile_datasize, iter_tile);
		tile_ptr += tile_datasize;
	}

	chrdef palview_tiledef {
		"",
		swatch_size,
		swatch_size,
		8,
		{STEP32(0, 8)},
		{STEP32(0, 8 * swatch_size)},
		{STEP32(0, 1)},
	};

	auto rendered_tiles = render_chrset(palview_tiledef, tile_buffer, tile_buffer.size(), {});
	rendered_tiles.palette(workpal);
	png::image<png::index_pixel> outimg(to_png(rendered_tiles));

	if (cfg.out_path.empty())
		outimg.write_stream(cout);
	else
		outimg.write(cfg.out_path);
}

void render_full_palette()
{
	// decode each palette line into a list of basic palettes
	vector<basic_palette> palette_lines;

	{
		ifstream paldata {ifstream_checked(cfg.paldata_name)};

		while (paldata.good())
		{
			size_t pal_size {work_paldef->datasize() / 8};
			byte_t palbuffer[pal_size];

			paldata.read(reinterpret_cast<char *>(palbuffer), pal_size);
			if (paldata.gcount() != pal_size)
				break;
			// TODO check if we got a full subpal's worth of data!
			decode_pal(*work_paldef, *work_coldef, palbuffer, &palette_lines.emplace_back());
		}

		if (palette_lines.empty())
			throw runtime_error("Could not read enough data for a complete color palette");
	}

	size_t const image_width {swatch_size * work_paldef->length()}, image_height {swatch_size * palette_lines.size()};
	size_t outimg_row_idx {0};

	vector<png::rgb_pixel> pixel_row(image_width);
	png::pixel_buffer<png::rgb_pixel> pixbuf(image_width, image_height);

	for (auto const & pal : palette_lines)
	{
		auto iter_color = pixel_row.begin();
		for (auto iter_color_index {0}; iter_color_index < work_paldef->length(); ++iter_color_index)
		{
			auto color = pal[iter_color_index];
			png::rgb_pixel rgb_color {color.red, color.green, color.blue};
			fill(iter_color, iter_color + swatch_size, rgb_color);
			iter_color += swatch_size;
		}
		for (auto iter_pixel_row {0}; iter_pixel_row < swatch_size; ++iter_pixel_row)
			pixbuf.put_row(outimg_row_idx++, pixel_row);
	}

	png::image<png::rgb_pixel> outimg(image_width, image_height);
	outimg.set_pixbuf(pixbuf);

	if (cfg.out_path.empty())
		outimg.write_stream(cout);
	else
		outimg.write(cfg.out_path);
}

int main(int argc, char ** argv)
{
	try
	{

		process_args(argc, argv);

		def_helper defs(cfg);

		work_coldef = defs.coldef;
		work_paldef = defs.paldef;
		
		if (cfg.full_pal)
			render_full_palette();
		else
			render_palette_line();

		return 0;
	}
	catch (exception const & e)
	{
		cerr << "Error: " << e.what() << '\n';
		return -1;
	}
}
