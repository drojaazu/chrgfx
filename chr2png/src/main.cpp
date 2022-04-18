#include "chrgfx.hpp"
#include "filesys.hpp"
#include "import_defs.hpp"
#include "parsing.hpp"
#include "shared.hpp"
#include <getopt.h>
#include <iostream>

#ifdef DEBUG
#include <chrono>
#endif

using namespace std;
using namespace chrgfx;

void process_args(int argc, char ** argv);

struct runtime_config_chr2png : runtime_config
{
	string chrdata_name;
	string paldata_name;
	render_config render_cfg;
	string out_path;
} cfg;

int main(int argc, char ** argv)
{
	try
	{
		/*******************************************************
		 *            SETUP & SANITY CHECKING
		 *******************************************************/
#ifdef DEBUG
		chrono::high_resolution_clock::time_point t1 =
				chrono::high_resolution_clock::now();
#endif
		process_args(argc, argv);

		// see if we have good input before moving on
		ifstream chrdata { ifstream_checked(cfg.chrdata_name.c_str()) };

		def_helper defs(cfg);

#ifdef DEBUG
		chrono::high_resolution_clock::time_point t2 =
				chrono::high_resolution_clock::now();
		auto duration =
				chrono::duration_cast<chrono::milliseconds>(t2 - t1).count();

		cerr << "SETUP: " << duration << "ms" << endl;
		cerr << "\tUsing gfxdefs file: " << cfg.gfxdefs_path << endl;
		cerr << "\tUsing chrdef '" << defs.chrdef->id() << "'" << endl;
		cerr << "\tUsing colrdef '" << defs.coldef->id() << "'" << endl;
		cerr << "\tUsing paldef '" << defs.paldef->id() << "'" << endl;
#endif

/*******************************************************
 *             TILE CONVERSION
 *******************************************************/
#ifdef DEBUG
		t1 = chrono::high_resolution_clock::now();
#endif

		size_t
				// byte size of one encoded tile
				in_chunksize { defs.chrdef->datasize() / (size_t)8 },
				// byte size of one basic (decoded) tile
				out_chunksize { (size_t)(defs.chrdef->width() *
																 defs.chrdef->height()) };

		// buffer for a single encoded tile, read from the stream
		char in_tile[in_chunksize];

		// basic tiles buffer
		buffer<byte_t> out_buffer(0);

		/*
			Some speed testing was done and, somewhat surprisingly, calling append
			on the buffer repeatedly was a bit faster than creating a large
			temporary buffer and resizing
		*/
		while(1)
		{
			chrdata.read(in_tile, in_chunksize);
			if(chrdata.eof())
				break;

			out_buffer.append(decode_chr(*defs.chrdef, in_tile), out_chunksize);
		}

#ifdef DEBUG
		t2 = chrono::high_resolution_clock::now();
		duration = chrono::duration_cast<chrono::milliseconds>(t2 - t1).count();

		cerr << "TILE CONVERSION: " << to_string(duration) << "ms" << endl;
#endif

/*******************************************************
 *                PALETTE CONVERSION
 *******************************************************/
#ifdef DEBUG
		t1 = chrono::high_resolution_clock::now();
#endif

		palette workpal;
		if(!cfg.paldata_name.empty())
		{
			ifstream paldata { ifstream_checked(cfg.paldata_name.c_str()) };

			size_t pal_size = defs.paldef->datasize() / 8;
			byte_t palbuffer[pal_size];
			paldata.read(palbuffer, pal_size);
			if(paldata.gcount() > pal_size)
				throw invalid_argument(
						"Input palette data too small to form a valid palette");

			workpal = decode_pal(*defs.paldef, *defs.coldef, palbuffer);
		}
		else
		{
			workpal = make_pal_random();
		}

#ifdef DEBUG
		t2 = chrono::high_resolution_clock::now();
		duration = chrono::duration_cast<chrono::milliseconds>(t2 - t1).count();

		cerr << "PALETTE GENERATION: " << duration << "ms" << endl;
#endif

#ifdef DEBUG
		t1 = chrono::high_resolution_clock::now();
#endif

		png::image<png::index_pixel> outimg { png_render(
				defs.chrdef->width(), defs.chrdef->height(), out_buffer, workpal,
				cfg.render_cfg) };

#ifdef DEBUG
		t2 = chrono::high_resolution_clock::now();
		duration = chrono::duration_cast<chrono::milliseconds>(t2 - t1).count();
		cerr << "PNG RENDER: " << duration << "ms" << endl;
#endif

#ifdef DEBUG
		t1 = chrono::high_resolution_clock::now();
#endif

		if(cfg.out_path.empty())
		{
			outimg.write_stream(cout);
		}
		else
		{
			outimg.write(cfg.out_path);
		}
#ifdef DEBUG
		t2 = chrono::high_resolution_clock::now();
		duration = chrono::duration_cast<chrono::milliseconds>(t2 - t1).count();
		cerr << "OUTPUT TO STREAM: " << duration << "ms" << endl;
#endif

		// everything's good, we're outta here
		return 0;
	}
	catch(exception const & e)
	{
		cerr << "Error: " << e.what() << endl;
		return -1;
	}
}

void process_args(int argc, char ** argv)
{
	// add chr2png specific options
	long_opts.push_back({ "chr-data", required_argument, nullptr, 'c' });
	long_opts.push_back({ "pal-data", required_argument, nullptr, 'p' });
	long_opts.push_back({ "trns", no_argument, nullptr, 't' });
	long_opts.push_back({ "trns-index", required_argument, nullptr, 'i' });
	long_opts.push_back({ "border", no_argument, nullptr, 'b' });
	long_opts.push_back({ "row-size", required_argument, nullptr, 'r' });
	long_opts.push_back({ "output", required_argument, nullptr, 'o' });
	long_opts.push_back({ 0, 0, 0, 0 });
	short_opts.append("ti:bd:c:p:s:o:");

	opt_details.push_back({ false, L"Path to input encoded tiles", nullptr });
	opt_details.push_back({ false, L"Path to input encoded palette", nullptr });
	opt_details.push_back({ false, L"Use palette transparency", nullptr });
	opt_details.push_back(
			{ false, L"Palette index to use for transparency", nullptr });
	opt_details.push_back({ false,
													L"Draw a 1 pixel border around tiles in output image",
													nullptr });
	opt_details.push_back(
			{ false, L"Number of tiles per row in output image", nullptr });
	opt_details.push_back({ false, L"Path to output PNG image", nullptr });

	// read/parse arguments
	while(true)
	{
		const auto this_opt =
				getopt_long(argc, argv, short_opts.data(), long_opts.data(), nullptr);
		if(this_opt == -1)
			break;

		// handle shared arguments
		if(shared_args(this_opt, cfg))
			continue;

		// handle chr2png specific arguments
		switch(this_opt)
		{
				// input tile data path
			case 'c':
				cfg.chrdata_name = optarg;
				break;

			// input palette data path
			case 'p':
				cfg.paldata_name = optarg;
				break;

			// use transparency in output png
			case 't':
				cfg.render_cfg.use_trns = true;
				break;

			// palette entry index for transparency
			case 'i':
				try
				{
					cfg.render_cfg.trns_index = stoi(optarg);
				}
				catch(const invalid_argument & e)
				{
					throw invalid_argument("Invalid transparency index value");
				}
				break;

			// draw border around tiles in output png
			case 'b':
				cfg.render_cfg.draw_border = true;
				break;

			// row size
			case 'd':
				try
				{
					cfg.render_cfg.row_size = stoi(optarg);
				}
				catch(const invalid_argument & e)
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
}
