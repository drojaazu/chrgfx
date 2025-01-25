#include "blob.hpp"
#include "fstreams.hpp"
#include "gfxdefman.hpp"
#include "imgfmt_png.hpp"
#include "setup.hpp"
#include <chrgfx/chrgfx.hpp>

#include <iostream>
#include <memory>

#ifdef DEBUG
#include <chrono>
#endif

using namespace std;
using namespace chrgfx;
using namespace motoi;

void process_args(int argc, char ** argv);

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

		gfxdef_manager defs(cfg);

		istream * chrdata;
		ifstream ifs;
		if (cfg.chrdata_path == "-")
		{
			chrdata = &cin;
		}
		else
		{
			// see if we have good input before moving on
			ifs = ifstream_checked(cfg.chrdata_path);
			chrdata = &ifs;
		}

#ifdef DEBUG
		chrono::high_resolution_clock::time_point t2 = chrono::high_resolution_clock::now();
		auto duration = chrono::duration_cast<chrono::milliseconds>(t2 - t1).count();
		cerr << "SETUP TIME: " << duration << "ms\n";
#endif

		/*******************************************************
		 *                PALETTE CONVERSION
		 *******************************************************/
		palette workpal;
		{
#ifdef DEBUG
			t1 = chrono::high_resolution_clock::now();
#endif
			if (! cfg.paldata_path.empty())
			{
				if (defs.paldef() == nullptr)
					throw runtime_error("no paldef loaded");
				if (defs.coldef() == nullptr)
					throw runtime_error("no coldef loaded");

				ifstream paldata {ifstream_checked(cfg.paldata_path)};
				size_t pal_size {defs.paldef()->datasize_bytes()};
				auto palbuffer {unique_ptr<byte_t>(new byte_t[pal_size])};

				paldata.seekg(cfg.pal_line * pal_size, ios::beg);
				paldata.read(reinterpret_cast<char *>(palbuffer.get()), pal_size);
				if (! paldata.good())
					throw runtime_error("Cannot read specified palette line index");

				decode_pal(*defs.paldef(), *defs.coldef(), palbuffer.get(), &workpal);
			}
			else
			{
				workpal = make_pal_random();
			}
#ifdef DEBUG
			t2 = chrono::high_resolution_clock::now();
			duration = chrono::duration_cast<chrono::milliseconds>(t2 - t1).count();

			cerr << "PALETTE GENERATION: " << duration << "ms\n";
#endif

#ifdef DEBUG
			t1 = chrono::high_resolution_clock::now();
#endif
		}

		/*******************************************************
		 *             TILE CONVERSION
		 *******************************************************/

		if (defs.chrdef() == nullptr)
			throw runtime_error("no chrdef loaded");

		blob out_buffer;
		{
#ifdef DEBUG
			t1 = chrono::high_resolution_clock::now();
#endif
			size_t
				// byte size of one encoded tile
				in_chunksize {(uint) (defs.chrdef()->datasize_bytes())},
				// byte size of one basic (decoded) tile
				out_chunksize {(size_t) (defs.chrdef()->width() * defs.chrdef()->height())};

			// buffer for a single encoded tile, read from the stream
			auto in_tile {unique_ptr<byte_t>(new byte_t[in_chunksize])},
				out_tile {unique_ptr<byte_t>(new pixel[out_chunksize])};

			/*
				Some speed testing was done and, somewhat surprisingly, calling append
				on the buffer repeatedly was a bit faster than creating a large
				temporary buffer and resizing
			*/
			while (true)
			{
				chrdata->read(reinterpret_cast<char *>(in_tile.get()), in_chunksize);
				if (! chrdata->good())
					break;

				decode_chr(*defs.chrdef(), in_tile.get(), out_tile.get());
				out_buffer.append(out_tile.get(), out_chunksize);
			}

#ifdef DEBUG
			t2 = chrono::high_resolution_clock::now();
			duration = chrono::duration_cast<chrono::milliseconds>(t2 - t1).count();

			cerr << "TILE CONVERSION: " << to_string(duration) << "ms\n";
#endif

#ifdef DEBUG
			t1 = chrono::high_resolution_clock::now();
#endif

			auto rendered_tiles = render_tileset(*defs.chrdef(), out_buffer, out_buffer.size(), cfg.render_cfg);
			rendered_tiles.color_map(workpal);
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
