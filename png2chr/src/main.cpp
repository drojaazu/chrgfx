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
		chrono::high_resolution_clock::time_point t1 =
				chrono::high_resolution_clock::now();
#endif
		process_args(argc, argv);

		// set up input data
		ifstream png_fstream;
		istream & png_data { (cfg.pngdata_name.empty() ? cin : png_fstream) };
		if(!cfg.pngdata_name.empty())
		{
			png_fstream.open(cfg.pngdata_name);
		}

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
 *             LOAD PNG
 *******************************************************/
#ifdef DEBUG
		t1 = chrono::high_resolution_clock::now();
#endif
		png::image<png::index_pixel> in_img(
				png_data, png::require_color_space<png::index_pixel>());

#ifdef DEBUG
		t2 = chrono::high_resolution_clock::now();
		duration = chrono::duration_cast<chrono::milliseconds>(t2 - t1).count();

		cerr << "LOAD PNG: " << to_string(duration) << "ms" << endl;
#endif

/*******************************************************
 *                 PNG CHUNK
 *******************************************************/
#ifdef DEBUG
		t1 = chrono::high_resolution_clock::now();
#endif

		// deal with tiles first
		if(!cfg.chr_outfile.empty())
		{
			buffer<byte_t> png_tiles { png_chunk(
					defs.chrdef->width(), defs.chrdef->height(), in_img.get_pixbuf()) };

#ifdef DEBUG
			t2 = chrono::high_resolution_clock::now();
			duration = chrono::duration_cast<chrono::milliseconds>(t2 - t1).count();

			cerr << "PNG CHUNK: " << to_string(duration) << "ms" << endl;
#endif

/*******************************************************
 *            TILE CONVERSION & OUTPUT
 *******************************************************/
#ifdef DEBUG
			t1 = chrono::high_resolution_clock::now();
#endif
			ofstream chr_outfile { ofstream_checked(cfg.chr_outfile.c_str()) };

			size_t chunksize { (unsigned)(defs.chrdef->datasize() / 8) };

			auto ptr_imgdata = png_tiles.data();
			auto ptr_imgdata_end = png_tiles.data() + png_tiles.datasize();

			while(ptr_imgdata != ptr_imgdata_end)
			{
				byte_t * temp_chr { encode_chr(*defs.chrdef, ptr_imgdata) };
				copy(temp_chr, temp_chr + chunksize,
						 ostream_iterator<byte_t>(chr_outfile));
				ptr_imgdata += defs.chrdef->width() * defs.chrdef->height();
				delete[] temp_chr;
			}
		}
#ifdef DEBUG
		t2 = chrono::high_resolution_clock::now();
		duration = chrono::duration_cast<chrono::milliseconds>(t2 - t1).count();

		cerr << "CHR ENCODE & OUTPUT TO STREAM: " << to_string(duration) << "ms"
				 << endl;
#endif

		// deal with the palette next
		if(!cfg.pal_outfile.empty())
		{

/*******************************************************
 *                PALETTE CONVERSION
 *******************************************************/
#ifdef DEBUG
			t1 = chrono::high_resolution_clock::now();
#endif

			auto paldef_palette_data { encode_pal(*defs.paldef, *defs.coldef,
																						in_img.get_palette()) };

#ifdef DEBUG
			t2 = chrono::high_resolution_clock::now();
			duration = chrono::duration_cast<chrono::milliseconds>(t2 - t1).count();

			cerr << "CONVERT PALETTE: " << to_string(duration) << "ms" << endl;
#endif

#ifdef DEBUG
			t1 = chrono::high_resolution_clock::now();
#endif

			ofstream pal_outfile { cfg.pal_outfile };
			if(!pal_outfile.good())
			{
				cerr << "pal-output error: " << strerror(errno) << endl;
			}

			// TODO: consider splitting the palette conversion routine into two
			// functions, on for subpal and one for full pal so we always know the
			// size of the data returned
			size_t filesize { (size_t)(defs.paldef->datasize() / 8) };

			pal_outfile.write((char *)(paldef_palette_data), filesize);

#ifdef DEBUG
			t2 = chrono::high_resolution_clock::now();
			duration = chrono::duration_cast<chrono::milliseconds>(t2 - t1).count();

			cerr << "PAL OUTPUT TO STREAM: " << to_string(duration) << "ms" << endl;
#endif
		}

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
	// add png2chr specific options
	long_opts.push_back({ "chr-output", required_argument, nullptr, 'c' });
	long_opts.push_back({ "pal-output", required_argument, nullptr, 'p' });
	long_opts.push_back({ "png-data", required_argument, nullptr, 'b' });
	long_opts.push_back({ 0, 0, 0, 0 });
	short_opts.append("c:p:b:");

	opt_details.push_back({ true, L"Path to output encoded tiles", nullptr });
	opt_details.push_back({ true, L"Path to output encoded palette", nullptr });
	opt_details.push_back({ true, L"Path to input PNG image", nullptr });

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

		// handle png2chr specific arguments
		switch(this_opt)
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
}
