#include "app.hpp"
#include "chrgfx.hpp"
#include "filesys.hpp"
#include "import_defs.hpp"
#include "parsing.hpp"
#include "shared.hpp"
#include <cerrno>
#include <getopt.h>
#include <iostream>
#include <map>
#include <stdio.h>

#ifdef DEBUG
#include <chrono>
#endif

using namespace chrgfx;
using namespace std;

bool process_args(int argc, char ** argv);
void print_help();

struct runtime_config_chr2png : runtime_config
{
	string chrdata_name;
	string paldata_name;
	render_config rendertraits;
	string out_path;
} cfg;

void init()
{
	long_opts.push_back({ "chr-data", required_argument, nullptr, 'c' });
	long_opts.push_back({ "pal-data", required_argument, nullptr, 'p' });
	long_opts.push_back({ "trns", no_argument, nullptr, 't' });
	long_opts.push_back({ "trns-index", required_argument, nullptr, 'i' });
	long_opts.push_back({ "border", no_argument, nullptr, 'b' });
	long_opts.push_back({ "row-size", required_argument, nullptr, 'r' });
	long_opts.push_back({ "output", required_argument, nullptr, 'o' });
	long_opts.push_back({ "help", no_argument, nullptr, 'h' });
	long_opts.push_back({ 0, 0, 0, 0 });
	short_opts.append("ti:bd:c:p:s:o:h");

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
	opt_details.push_back({ false, L"Display program usage", nullptr });
}

int main(int argc, char ** argv)
{
	try
	{
#ifdef DEBUG
		std::chrono::high_resolution_clock::time_point t1 =
				std::chrono::high_resolution_clock::now();
#endif
		try
		{
			// Runtime State Setup
			init();
			if(process_args(argc, argv))
			{
				return 0;
			}
		}
		catch(std::exception const & e)
		{
			// failure in argument parsing/runtime config
			std::cerr << "Invalid argument: " << e.what() << std::endl;
			return -5;
		}

		// see if we have good input before moving on
		ifstream chrdata { ifstream_checked(cfg.chrdata_name.c_str()) };

		// load definitions
		auto gfxdefs { load_gfxdefs(cfg.gfxdefs_path) };

		// there isn't really an elegant way of dealing with the custom converters
		// since they do not conform to a function signature or interface, etc. For
		// now, we represent each of the converters with a unique id (similar to the
		// ids in the gfxdefs file) and will call them in a case.

		// the converters are, at least, grouped by similar type: converters dealing
		// with tiles are equivalent to chrdefs, those dealing with palettes are
		// equivalent to paldefs, etc.

		// The lists below are used to check whether an id specified by the user is
		// one of the custom converters if it didn't match the ids imported from the
		// gfxdefs file
		vector<string> custom_conv_chr { "decode_chr_nintendo_sfc_3bpp" };

		vector<string> custom_conv_pal { "decode_pal_tilelayerpro",
																		 "encode_pal_tilelayerpro",
																		 "decode_pal_paintshoppro",
																		 "encode_pal_paintshoppro" };

		// no custom color converters, yet
		// vector<string> custom_conv_col {};

		string chrdef_id, coldef_id, paldef_id;

		auto profile_iter { gfxdefs.profiles.find(cfg.profile) };
		if(profile_iter != gfxdefs.profiles.end())
		{
			gfxprofile profile { profile_iter->second };
			chrdef_id = profile.chrdef_id();
			coldef_id = profile.coldef_id();
			paldef_id = profile.paldef_id();
		}

		// specific gfxdefs override profile settings
		if(!cfg.chrdef_id.empty())
		{
			chrdef_id = cfg.chrdef_id;
		}
		if(!cfg.coldef_id.empty())
		{
			coldef_id = cfg.coldef_id;
		}
		if(!cfg.paldef_id.empty())
		{
			paldef_id = cfg.paldef_id;
		}

		// sanity check - chrdef is required
		if(chrdef_id.empty())
		{
			std::cerr << "Must specify a tile definition (--chr-def or --profile)"
								<< std::endl;
			return -8;
		}

		auto i_find_chrdef { gfxdefs.chrdefs.find(chrdef_id) };
		if(i_find_chrdef == gfxdefs.chrdefs.end())
		{
			std::cerr << "Could not find specified chrdef" << std::endl;
			return -9;
		}

		chrgfx::chrdef chrdef { i_find_chrdef->second };

		// sanity check - coldef not required - if not specified, use default
		// built-in
		if(coldef_id.empty())
		{
			coldef_id = "basic_8bit_random";
		}

		chrgfx::coldef * coldef;

		auto i_find_rgbcoldef { gfxdefs.rgbcoldefs.find(coldef_id) };
		if(i_find_rgbcoldef != gfxdefs.rgbcoldefs.end())
		{
			coldef = (chrgfx::coldef *)&i_find_rgbcoldef->second;
		}
		else
		{
			auto i_find_refcoldef { gfxdefs.refcoldefs.find(coldef_id) };
			if(i_find_refcoldef != gfxdefs.refcoldefs.end())
			{
				coldef = (chrgfx::coldef *)&i_find_refcoldef->second;
			}
			else
			{
				std::cerr << "Could not find specified coldef" << std::endl;
				return -10;
			}
		}

		// sanity check - paldef not required - if not specified, use default
		// built-in
		if(paldef_id.empty())
		{
			paldef_id = "basic_256color";
		}

		auto paldef_iter { gfxdefs.paldefs.find(paldef_id) };
		if(paldef_iter == gfxdefs.paldefs.end())
		{
			std::cerr << "Could not find specified paldef" << std::endl;
			return -11;
		}

		paldef paldef { paldef_iter->second };

#ifdef DEBUG
		std::chrono::high_resolution_clock::time_point t2 =
				std::chrono::high_resolution_clock::now();
		auto duration =
				std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count();

		std::cerr << "SETUP: " << duration << "ms" << std::endl;
		std::cerr << "Using chrdef '" << chrdef_id << "'" << std::endl;
		std::cerr << "Using colrdef '" << coldef_id << "'" << std::endl;
		std::cerr << "Using paldef '" << paldef_id << "'" << std::endl;
#endif

/*******************************************************
 *             TILE CONVERSION
 *******************************************************/
#ifdef DEBUG
		t1 = std::chrono::high_resolution_clock::now();
#endif

		size_t
				// byte size of one encoded tile
				in_chunksize { chrdef.datasize() / (size_t)8 },
				// byte size of one basic (decoded) tile
				out_chunksize { (size_t)(chrdef.width() * chrdef.height()) };

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

			out_buffer.append(decode_chr(chrdef, in_tile), out_chunksize);
		}

#ifdef DEBUG
		t2 = std::chrono::high_resolution_clock::now();
		duration =
				std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count();

		std::cerr << "TILE CONVERSION: " << std::to_string(duration) << "ms"
							<< std::endl;
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

			size_t pal_size = paldef.datasize() / 8;
			byte_t palbuffer[pal_size];
			paldata.read(palbuffer, pal_size);
			if(paldata.gcount() > pal_size)
				throw std::invalid_argument(
						"Input palette data too small to form a valid palette");

			workpal = decode_pal(paldef, *coldef, palbuffer);
		}
		else
		{
			workpal = make_pal_random();
		}

#ifdef DEBUG
		t2 = std::chrono::high_resolution_clock::now();
		duration =
				std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count();

		std::cerr << "PALETTE GENERATION: " << duration << "ms" << std::endl;
#endif

#ifdef DEBUG
		t1 = std::chrono::high_resolution_clock::now();
#endif

		png::image<png::index_pixel> outimg { png_render(
				chrdef.width(), chrdef.height(), out_buffer, workpal,
				cfg.rendertraits) };

#ifdef DEBUG
		t2 = std::chrono::high_resolution_clock::now();
		duration =
				std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count();
		std::cerr << "PNG RENDER: " << duration << "ms" << std::endl;
#endif

#ifdef DEBUG
		t1 = std::chrono::high_resolution_clock::now();
#endif

		if(cfg.out_path.empty())
		{
			outimg.write_stream(std::cout);
		}
		else
		{
			outimg.write(cfg.out_path);
		}
#ifdef DEBUG
		t2 = std::chrono::high_resolution_clock::now();
		duration =
				std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count();
		std::cerr << "OUTPUT TO STREAM: " << duration << "ms" << std::endl;
#endif

		// everything's good, we're outta here
		return 0;
	}
	catch(std::exception const & e)
	{
		std::cerr << "Error: " << e.what() << std::endl;
		return -1;
	}
}

bool process_args(int argc, char ** argv)
{

	bool default_processed = process_default_args(cfg, argc, argv);

	if(!default_processed)
	{
		print_help();
		exit(1);
	}

	while(true)
	{
		const auto this_opt =
				getopt_long(argc, argv, short_opts.data(), long_opts.data(), nullptr);
		if(this_opt == -1)
			break;

		switch(this_opt)
		{
				// chr-data
			case 'c':
				cfg.chrdata_name = optarg;
				break;

			// pal-data
			case 'p':
				cfg.paldata_name = optarg;
				break;

			// trns
			case 't':
				cfg.rendertraits.use_trns = true;
				break;

			// trns entry
			case 'i':
				try
				{
					cfg.rendertraits.trns_index = std::stoi(optarg);
				}
				catch(const std::invalid_argument & e)
				{
					throw std::invalid_argument("Invalid transparency index value");
				}
				break;

			// draw tile border
			case 'b':
				cfg.rendertraits.draw_border = true;
				break;

			// columns
			case 'd':
				try
				{
					cfg.rendertraits.row_size = std::stoi(optarg);
				}
				catch(const std::invalid_argument & e)
				{
					throw std::invalid_argument("Invalid columns value");
				}
				break;

			// output
			case 'o':
				cfg.out_path = optarg;
				break;

			case 'h':
				print_help();
				return true;
		}
	}

	return false;
}

void print_help() {}
