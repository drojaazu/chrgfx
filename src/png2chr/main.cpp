#include "fstreams.hpp"
#include "gfxdefman.hpp"
#include "shared.hpp"
#include <chrgfx/chrgfx.hpp>
#include <getopt.h>
#include <iostream>
#include <iterator>

#ifdef DEBUG
#include <chrono>
#endif

using namespace std;
using namespace chrgfx;
using namespace motoi;

void process_args(int argc, char ** argv);

struct runtime_config_png2chr : runtime_config
{
	string pngdata_name;
	string chr_outfile;
	string pal_outfile;
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

		// set up input data
		ifstream png_fstream;
		istream & png_data {(cfg.pngdata_name.empty() ? cin : png_fstream)};
		if (! cfg.pngdata_name.empty())
		{
			png_fstream.open(cfg.pngdata_name);
		}

#ifdef DEBUG
		chrono::high_resolution_clock::time_point t2 = chrono::high_resolution_clock::now();
		auto duration = chrono::duration_cast<chrono::milliseconds>(t2 - t1).count();
		cerr << "SETUP TIME: " << duration << "ms\n";
#endif

		/*******************************************************
		 *             LOAD IMAGE
		 *******************************************************/

#ifdef DEBUG
		t1 = chrono::high_resolution_clock::now();
#endif

		auto image_data = from_png({png_data, png::require_color_space<png::index_pixel>()});

#ifdef DEBUG
		t2 = chrono::high_resolution_clock::now();
		duration = chrono::duration_cast<chrono::milliseconds>(t2 - t1).count();

		cerr << "LOAD PNG: " << to_string(duration) << "ms\n";
#endif

		if (! cfg.chr_outfile.empty())
		{

			/*******************************************************
			 *                 TILE SEGMENTATION
			 *******************************************************/

#ifdef DEBUG
			t1 = chrono::high_resolution_clock::now();
#endif

			auto tile_width {image_data.width() / defs.chrdef()->width()},
				tile_height {image_data.height() / defs.chrdef()->height()},
				chr_datasize {defs.chrdef()->width() * defs.chrdef()->height()},
				chrset_datasize {tile_width * tile_height * chr_datasize};
			vector<byte_t> tileset_data(chrset_datasize);
			make_chrset(*defs.chrdef(), image_data, tileset_data.data());

#ifdef DEBUG
			t2 = chrono::high_resolution_clock::now();
			duration = chrono::duration_cast<chrono::milliseconds>(t2 - t1).count();

			cerr << "TILE SEGMENTATION: " << to_string(duration) << "ms\n";
			cerr << "TILE COUNT: " << (tileset_data.size() / (defs.chrdef()->width() * defs.chrdef()->height())) << '\n';
#endif

			/*******************************************************
			 *            TILE CONVERSION & OUTPUT
			 *******************************************************/

#ifdef DEBUG
			t1 = chrono::high_resolution_clock::now();
#endif
			auto chr_outfile {ofstream_checked(cfg.chr_outfile)};

			size_t in_chunksize {(size_t) (defs.chrdef()->width() * defs.chrdef()->height())},
				out_chunksize {(uint) (defs.chrdef()->datasize() / 8)};

			auto ptr_in_tile = tileset_data.data();
			byte_t * ptr_imgdata_end = ptr_in_tile + tileset_data.size();

			auto out_tile = new basic_pixel[in_chunksize];
			while (ptr_in_tile != ptr_imgdata_end)
			{
				// TODO create a cache and use the out pointer on encode_chr
				encode_chr(defs.chrdef(), ptr_in_tile, out_tile);
				copy(out_tile, out_tile + out_chunksize, ostream_iterator<char>(chr_outfile));
				ptr_in_tile += in_chunksize;
			}
			delete[] out_tile;

#ifdef DEBUG
			t2 = chrono::high_resolution_clock::now();
			duration = chrono::duration_cast<chrono::milliseconds>(t2 - t1).count();

			cerr << "TILE ENCODE/OUTPUT: " << to_string(duration) << "ms\n";
#endif
		}

		/*******************************************************
		 *                PALETTE CONVERSION
		 *******************************************************/
		if (! cfg.pal_outfile.empty())
		{

#ifdef DEBUG
			t1 = chrono::high_resolution_clock::now();
#endif

			auto paldef_palette_data {new byte_t[defs.paldef()->datasize() >> 3]};
			encode_pal(defs.paldef(), defs.coldef(), image_data.palette(), paldef_palette_data);

#ifdef DEBUG
			t2 = chrono::high_resolution_clock::now();
			duration = chrono::duration_cast<chrono::milliseconds>(t2 - t1).count();

			cerr << "CONVERT PALETTE: " << to_string(duration) << "ms\n";
#endif

#ifdef DEBUG
			t1 = chrono::high_resolution_clock::now();
#endif

			ofstream pal_outfile {cfg.pal_outfile};
			if (! pal_outfile.good())
				cerr << "pal-output error: " << strerror(errno) << '\n';

			// TODO consider splitting the palette conversion routine into two
			// functions, on for subpal and one for full pal so we always know the
			// size of the data returned
			size_t filesize {(size_t) (defs.paldef()->datasize() / 8)};

			pal_outfile.write(reinterpret_cast<char *>(paldef_palette_data), filesize);
			delete[] paldef_palette_data;

#ifdef DEBUG
			t2 = chrono::high_resolution_clock::now();
			duration = chrono::duration_cast<chrono::milliseconds>(t2 - t1).count();

			cerr << "PAL OUTPUT TO STREAM: " << to_string(duration) << "ms\n";
#endif
		}

		// everything's good, we're outta here
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
	// add png2chr specific options
	long_opts.push_back({"chr-output", required_argument, nullptr, 'c'});
	long_opts.push_back({"pal-output", required_argument, nullptr, 'p'});
	long_opts.push_back({"png-data", required_argument, nullptr, 'b'});
	long_opts.push_back({nullptr, 0, nullptr, 0});
	short_opts.append("c:p:b:");

	opt_details.push_back({true, "Path to output encoded tiles", nullptr});
	opt_details.push_back({true, "Path to output encoded palette", nullptr});
	opt_details.push_back({true, "Path to input PNG image", nullptr});

	// read/parse arguments
	while (true)
	{
		const auto this_opt = getopt_long(argc, argv, short_opts.data(), long_opts.data(), nullptr);
		if (this_opt == -1)
			break;

		// handle shared arguments
		if (shared_args(this_opt, cfg))
			continue;

		// handle png2chr specific arguments
		switch (this_opt)
		{
			// chr-output
			case 'c':
				cfg.chr_outfile = optarg;
				break;

			// pal-output
			case 'p':
				cfg.pal_outfile = optarg;
				break;

			// png-data
			case 'b':
				cfg.pngdata_name = optarg;
				break;
		}
	}

	if (cfg.gfxdefs_path.empty())
		cfg.gfxdefs_path = get_gfxdefs_path();

#ifdef DEBUG
	cerr << "\tUsing gfxdefs file: " << cfg.gfxdefs_path << '\n';
#endif
}
