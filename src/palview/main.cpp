#include "blob.hpp"
#include "fstreams.hpp"
#include "imgfmt_png.hpp"
#include "shared.hpp"
#include <chrgfx/chrgfx.hpp>
#include <getopt.h>
#include <iostream>
#include <png++/rgb_pixel.hpp>

#ifdef DEBUG
#include <chrono>
#endif

using namespace std;
using namespace chrgfx;
using namespace motoi;

uint const swatch_size {32};

chrgfx::coldef const * work_coldef {&chrgfx::gfxdefs::col_bgr_333_packed};
chrgfx::paldef const * work_paldef {&chrgfx::gfxdefs::pal_16bit_256color};

struct runtime_config_palview : runtime_config
{
	string paldata_name;
	string out_path;
	uint pal_line {0};
	bool full_pal {false};
	render_config render_cfg;
} cfg;

void process_args(int argc, char ** argv);

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
		// if (! paldata.good())
		//	throw runtime_error("Cannot read specified palette line index");

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

	auto rendered_tiles = render_chrset(palview_tiledef, tile_buffer, tile_buffer.size(), cfg.render_cfg);
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
	}

	// each palette line

	// for each palette line
	// - vector of direct colors for this row
	// - for each pixel line
	// -- for each swatch
	// --- render (swatch width) pixels
	//

	size_t const image_width {swatch_size * work_paldef->length()}, image_height {swatch_size * palette_lines.size()};
	size_t full_pixel_row {0};

	vector<png::rgb_pixel> pxlrow_work(image_width);
	png::pixel_buffer<png::rgb_pixel> png_pixbuf(image_width, image_height);

	for (auto const & pal : palette_lines)
	{
		auto iter_color = pxlrow_work.begin();
		for (auto iter_color_index {0}; iter_color_index < work_paldef->length(); ++iter_color_index)
		{
			auto color = pal[iter_color_index];
			png::rgb_pixel rgb_color {color.red, color.green, color.blue};
			fill(iter_color, iter_color + swatch_size, rgb_color);
			iter_color += swatch_size;
		}
		for (auto iter_pixel_row {0}; iter_pixel_row < swatch_size; ++iter_pixel_row)
			png_pixbuf.put_row(full_pixel_row++, pxlrow_work);
	}

	png::image<png::rgb_pixel> outimg(image_width, image_height);
	outimg.set_pixbuf(png_pixbuf);

	if (cfg.out_path.empty())
		outimg.write_stream(cout);
	else
		outimg.write(cfg.out_path);
}

int main(int argc, char ** argv)
{
	try
	{
		/*******************************************************
		 *            SETUP & SANITY CHECKING
		 *******************************************************/

#ifdef DEBUG
		chrono::high_resolution_clock::time_point t1 = chrono::high_resolution_clock::now();
#endif
		process_args(argc, argv);

		def_helper defs(cfg);

		work_coldef = defs.coldef;
		work_paldef = defs.paldef;

		if (cfg.list_gfxdefs)
		{
			defs.list_gfxdefs(cout);
			exit(0);
		}

#ifdef DEBUG
		chrono::high_resolution_clock::time_point t2 = chrono::high_resolution_clock::now();
		auto duration = chrono::duration_cast<chrono::milliseconds>(t2 - t1).count();
		cerr << "SETUP TIME: " << duration << "ms\n";
#endif

		if (cfg.full_pal)
		{
#ifdef DEBUG
			t1 = chrono::high_resolution_clock::now();
#endif

			render_full_palette();
#ifdef DEBUG
			t2 = chrono::high_resolution_clock::now();
			duration = chrono::duration_cast<chrono::milliseconds>(t2 - t1).count();
			cerr << "RENDER FULL PALETTE: " << to_string(duration) << "ms\n";
#endif
		}
		else
		{
#ifdef DEBUG
			t1 = chrono::high_resolution_clock::now();
#endif
			render_palette_line();
#ifdef DEBUG
			t2 = chrono::high_resolution_clock::now();
			duration = chrono::duration_cast<chrono::milliseconds>(t2 - t1).count();
			cerr << "RENDER PALETTE LINE: " << to_string(duration) << "ms\n";
#endif
		}

		return 0;
	}
	catch (exception const & e)
	{
		cerr << "Error: " << e.what() << '\n';
		return -1;
	}
}

void process_args(int argc, char ** argv)
{
	// add palview specific options
	long_opts.push_back({"pal-data", required_argument, nullptr, 'p'});
	long_opts.push_back({"pal-line", required_argument, nullptr, 'l'});
	long_opts.push_back({"full-pal", no_argument, nullptr, 'f'});
	long_opts.push_back({"row-size", required_argument, nullptr, 'r'});
	long_opts.push_back({"output", required_argument, nullptr, 'o'});
	long_opts.push_back({nullptr, 0, nullptr, 0});
	short_opts.append("p:l:fr:o:");

	opt_details.push_back({false, "Path to input encoded palette", nullptr});
	opt_details.push_back({false, "Palette line to render", nullptr});
	opt_details.push_back({false, "Render all palette data instead of a single palette line", nullptr});
	opt_details.push_back({false, "Number of tiles per row in output image", nullptr});
	opt_details.push_back({false, "Path to output image", nullptr});

	// read/parse arguments
	while (true)
	{
		const auto this_opt = getopt_long(argc, argv, short_opts.data(), long_opts.data(), nullptr);
		if (this_opt == -1)
			break;

		// handle shared arguments
		if (shared_args(this_opt, cfg))
			continue;

		// handle chr2png specific arguments
		switch (this_opt)
		{
			// input palette data path
			case 'p':
				cfg.paldata_name = optarg;
				break;

				// palette line
			case 'l':
				try
				{
					auto pal_line {stoi(optarg)};
					if (pal_line < 1)
						throw invalid_argument("Invalid palette line value");
					// user input is indexed from 1, convert to 0 index here
					cfg.pal_line = pal_line - 1;
				}
				catch (const invalid_argument & e)
				{
					throw invalid_argument("Invalid palette line value");
				}
				break;

			case 'f':
				cfg.full_pal = true;
				break;

			// row size
			case 'r':
				try
				{
					cfg.render_cfg.row_size = stoi(optarg);
				}
				catch (const invalid_argument & e)
				{
					throw invalid_argument("Invalid columns value");
				}
				break;

			// png output path
			case 'o':
				cfg.out_path = optarg;
				break;
		}
	}

	if (cfg.gfxdefs_path.empty())
		cfg.gfxdefs_path = get_gfxdefs_path();

#ifdef DEBUG
	cerr << "\tUsing gfxdefs file: " << cfg.gfxdefs_path << '\n';
#endif
}
