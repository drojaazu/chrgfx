#include "blob.hpp"
#include "fstreams.hpp"
#include "gfxdefman.hpp"
#include "imgfmt_png.hpp"
#include "shared.hpp"
#include <chrgfx/chrgfx.hpp>
#include <getopt.h>
#include <iostream>

#ifdef DEBUG
#include <chrono>
#endif

using namespace std;
using namespace chrgfx;
using namespace motoi;

void process_args(int argc, char ** argv);

struct runtime_config_chr2png : runtime_config
{
	string chrdata_name;
	string paldata_name;
	render_config render_cfg;
	string out_path;
	uint pal_line {0};
} cfg;

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

		gfxdef_manager defs;
		defs.load_gfxdefs(cfg);

		istream * chrdata;
		ifstream ifs;
		if (cfg.chrdata_name == "-")
		{
			chrdata = &cin;
		}
		else
		{
			// see if we have good input before moving on
			ifs = ifstream_checked(cfg.chrdata_name);
			chrdata = &ifs;
		}

#ifdef DEBUG
		chrono::high_resolution_clock::time_point t2 = chrono::high_resolution_clock::now();
		auto duration = chrono::duration_cast<chrono::milliseconds>(t2 - t1).count();
		cerr << "SETUP TIME: " << duration << "ms\n";
#endif

		// basic gfx buffer
		blob out_buffer;

		/*******************************************************
		 *             TILE CONVERSION
		 *******************************************************/
		{
#ifdef DEBUG
			t1 = chrono::high_resolution_clock::now();
#endif
			size_t
				// byte size of one encoded tile
				in_chunksize {(uint) (defs.chrdef()->datasize() / 8)},
				// byte size of one basic (decoded) tile
				out_chunksize {(size_t) (defs.chrdef()->width() * defs.chrdef()->height())};

			// buffer for a single encoded tile, read from the stream
			auto in_tile = new byte_t[in_chunksize], out_tile = new basic_pixel[out_chunksize];

			/*
				Some speed testing was done and, somewhat surprisingly, calling append
				on the buffer repeatedly was a bit faster than creating a large
				temporary buffer and resizing
			*/
			while (true)
			{
				chrdata->read(reinterpret_cast<char *>(in_tile), in_chunksize);
				if (! chrdata->good())
					break;

				decode_chr(*defs.chrdef(), in_tile, out_tile);
				out_buffer.append(out_tile, out_chunksize);
			}
			delete[] out_tile;
			delete[] in_tile;

#ifdef DEBUG
			t2 = chrono::high_resolution_clock::now();
			duration = chrono::duration_cast<chrono::milliseconds>(t2 - t1).count();

			cerr << "TILE CONVERSION: " << to_string(duration) << "ms\n";
#endif
		}

		/*******************************************************
		 *                PALETTE CONVERSION
		 *******************************************************/
		basic_palette workpal;
		{
#ifdef DEBUG
			t1 = chrono::high_resolution_clock::now();
#endif
			if (! cfg.paldata_name.empty())
			{
				ifstream paldata {ifstream_checked(cfg.paldata_name)};
				size_t pal_size {defs.paldef()->datasize() / 8};
				byte_t palbuffer[pal_size];

				paldata.seekg(cfg.pal_line * pal_size, ios::beg);
				paldata.read(reinterpret_cast<char *>(palbuffer), pal_size);
				if (! paldata.good())
					throw runtime_error("Cannot read specified palette line index");

				decode_pal(*defs.paldef(), *defs.coldef(), palbuffer, &workpal);
			}
			else
			{
				workpal = make_pal_random();
			}

			/*******************************************************
			 *              RENDER TILESET & WRITE FILE
			 *******************************************************/
			{
#ifdef DEBUG
				t2 = chrono::high_resolution_clock::now();
				duration = chrono::duration_cast<chrono::milliseconds>(t2 - t1).count();

				cerr << "PALETTE GENERATION: " << duration << "ms\n";
#endif
			}

#ifdef DEBUG
			t1 = chrono::high_resolution_clock::now();
#endif

			auto rendered_tiles = render_chrset(*defs.chrdef(), out_buffer, out_buffer.size(), cfg.render_cfg);
			rendered_tiles.palette(workpal);
			png::image<png::index_pixel> outimg {to_png(rendered_tiles, cfg.render_cfg.trns_index)};

#ifdef DEBUG
			t2 = chrono::high_resolution_clock::now();
			duration = chrono::duration_cast<chrono::milliseconds>(t2 - t1).count();
			cerr << "PNG RENDER: " << duration << "ms\n";
#endif

#ifdef DEBUG
			t1 = chrono::high_resolution_clock::now();
#endif

			if (cfg.out_path.empty())
				outimg.write_stream(cout);
			else
				outimg.write(cfg.out_path);

#ifdef DEBUG
			t2 = chrono::high_resolution_clock::now();
			duration = chrono::duration_cast<chrono::milliseconds>(t2 - t1).count();
			cerr << "OUTPUT TO STREAM: " << duration << "ms\n";
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
	// add chr2png specific options
	long_opts.push_back({"chr-data", required_argument, nullptr, 'c'});
	long_opts.push_back({"pal-data", required_argument, nullptr, 'p'});
	long_opts.push_back({"pal-line", required_argument, nullptr, 'l'});
	long_opts.push_back({"trns-index", required_argument, nullptr, 'i'});
	long_opts.push_back({"row-size", required_argument, nullptr, 'r'});
	long_opts.push_back({"output", required_argument, nullptr, 'o'});
	long_opts.push_back({nullptr, 0, nullptr, 0});
	short_opts.append("c:p:l:i:r:o:");

	opt_details.push_back({false, "Path to input encoded tiles", nullptr});
	opt_details.push_back({false, "Path to input encoded palette", nullptr});
	opt_details.push_back({false, "Palette line to use for PNG output", nullptr});
	opt_details.push_back({false, "Palette index to use for transparency", nullptr});
	opt_details.push_back({false, "Number of tiles per row in output image", nullptr});
	opt_details.push_back({false, "Path to output PNG image", nullptr});

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
			// input tile data path
			case 'c':
				cfg.chrdata_name = optarg;
				break;

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

			// palette entry index for transparency
			case 'i':
				try
				{
					string s = optarg;
					auto trns_index {stoi(s)};
					if (trns_index < 0)
						throw invalid_argument("Invalid transparency index value");
					cfg.render_cfg.trns_index = trns_index;
				}
				catch (const invalid_argument & e)
				{
					throw invalid_argument("Invalid transparency index value");
				}
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
