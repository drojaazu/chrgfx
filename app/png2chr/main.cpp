#include "filesys.hpp"
#include "gfxdefman.hpp"
#include "setup.hpp"
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

int main(int argc, char ** argv)
{
#ifdef DEBUG
	chrono::high_resolution_clock::time_point t1, t2;
#endif

	try
	{
		/*******************************************************
		 *            SETUP & SANITY CHECKING
		 *******************************************************/

#ifdef DEBUG
		t1 = chrono::high_resolution_clock::now();
#endif
		process_args(argc, argv);

		gfxdef_manager defs(cfg);
		istream * png_data;
		ifstream ifs_png_data;
		if (cfg.pngdata_path.empty())
		{
			png_data = &cin;
		}
		else
		{
			ifs_png_data = ifstream_checked(cfg.pngdata_path);
			png_data = &ifs_png_data;
		}

#ifdef DEBUG
		t2 = chrono::high_resolution_clock::now();
		auto duration = chrono::duration_cast<chrono::milliseconds>(t2 - t1).count();
		cerr << "SETUP: " << duration << "ms\n";
#endif

		/*******************************************************
		 *             LOAD IMAGE
		 *******************************************************/

#ifdef DEBUG
		t1 = chrono::high_resolution_clock::now();
#endif

		auto image_data {from_png({*png_data, png::require_color_space<png::index_pixel>()})};

#ifdef DEBUG
		t2 = chrono::high_resolution_clock::now();
		duration = chrono::duration_cast<chrono::milliseconds>(t2 - t1).count();

		cerr << "LOAD PNG: " << to_string(duration) << "ms\n";
#endif

		/*******************************************************
		 *                 TILE SEGMENTATION
		 *******************************************************/

		if (! cfg.out_chrdata_path.empty())
		{
			if (defs.chrdef() == nullptr)
				throw runtime_error("no chrdef loaded");

#ifdef DEBUG
			t1 = chrono::high_resolution_clock::now();
#endif

			auto tile_columns {image_data.width() / defs.chrdef()->width()},
				tile_rows {image_data.height() / defs.chrdef()->height()},
				chr_datasize {defs.chrdef()->width() * defs.chrdef()->height()},
				tileset_datasize {tile_columns * tile_rows * chr_datasize};
			vector<byte_t> tileset_data(tileset_datasize);
			make_tileset(*defs.chrdef(), image_data, tileset_data.data());

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
			auto chr_outfile {ofstream_checked(cfg.out_chrdata_path)};

			size_t in_chunksize {(size_t) (defs.chrdef()->width() * defs.chrdef()->height())},
				out_chunksize {(uint) (defs.chrdef()->datasize_bytes())};

			auto ptr_in_tile = tileset_data.data();
			byte_t * ptr_imgdata_end = ptr_in_tile + tileset_data.size();

			auto out_tile {unique_ptr<pixel>(new pixel[in_chunksize])};
			while (ptr_in_tile != ptr_imgdata_end)
			{
				// TODO create a cache and use the out pointer on encode_chr
				encode_chr(*defs.chrdef(), ptr_in_tile, out_tile.get());
				copy(out_tile.get(), out_tile.get() + out_chunksize, ostream_iterator<char>(chr_outfile));
				ptr_in_tile += in_chunksize;
			}

#ifdef DEBUG
			t2 = chrono::high_resolution_clock::now();
			duration = chrono::duration_cast<chrono::milliseconds>(t2 - t1).count();

			cerr << "TILE ENCODE/OUTPUT: " << to_string(duration) << "ms\n";
#endif
		}

		/*******************************************************
		 *                PALETTE CONVERSION
		 *******************************************************/
		if (! cfg.out_paldata_path.empty())
		{
			if (defs.paldef() == nullptr)
				throw runtime_error("no paldef loaded");
			if (defs.coldef() == nullptr)
				throw runtime_error("no coldef loaded");

#ifdef DEBUG
			t1 = chrono::high_resolution_clock::now();
#endif

			auto paldef_palette_data {unique_ptr<byte_t>(new byte_t[defs.paldef()->datasize() >> 3])};
			encode_pal(*defs.paldef(), *defs.coldef(), image_data.color_map(), paldef_palette_data.get());

#ifdef DEBUG
			t2 = chrono::high_resolution_clock::now();
			duration = chrono::duration_cast<chrono::milliseconds>(t2 - t1).count();

			cerr << "CONVERT PALETTE: " << to_string(duration) << "ms\n";
#endif

#ifdef DEBUG
			t1 = chrono::high_resolution_clock::now();
#endif

			ofstream pal_outfile {ofstream_checked(cfg.out_paldata_path)};
			pal_outfile.write(reinterpret_cast<char *>(paldef_palette_data.get()), defs.paldef()->datasize_bytes());

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
