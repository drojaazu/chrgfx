#include "chrgfx.hpp"
#include "filesys.hpp"
#include "import_defs.hpp"
#include "shared.hpp"
#include "usage.hpp"
#include "parsing.hpp"
#include <cerrno>
#include <getopt.h>
#include <iostream>
#include <iterator>
#include <stdio.h>

#ifdef DEBUG
#include <chrono>
#endif

using namespace std;
using namespace chrgfx;

bool process_args(int argc, char ** argv);
void print_help();

struct runtime_config_png2chr : runtime_config
{
	string pngdata_name { "" };
	string chr_outfile { "" };
	string pal_outfile { "" };
} cfg;

void init()
{
	long_opts.push_back({ "chr-output", required_argument, nullptr, 'c' });
	long_opts.push_back({ "pal-output", required_argument, nullptr, 'p' });
	long_opts.push_back({ "png-data", required_argument, nullptr, 'b' });
	long_opts.push_back({ "help", no_argument, nullptr, 'h' });
	long_opts.push_back({ 0, 0, 0, 0 });
	short_opts.append("c:p:b:h");

	opt_details.push_back({ true, L"Path to output encoded tiles", nullptr });
	opt_details.push_back({ true, L"Path to output encoded palette", nullptr });
	opt_details.push_back({ true, L"Path to input PNG image", nullptr });
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
			init();
			// Runtime State Setup
			if(process_args(argc, argv))
			{
				return 0;
			};
		}
		catch(std::exception const & e)
		{
			std::cerr << "Invalid argument: " << e.what() << std::endl;
			return -5;
		}

		// set up input data
		std::ifstream png_fstream;
		std::istream & png_data =
				(cfg.pngdata_name.empty() ? std::cin : png_fstream);

		if(!cfg.pngdata_name.empty())
			png_fstream = ifstream_checked(cfg.pngdata_name.c_str());

		// load definitions
		auto gfxdefs { load_gfxdefs(cfg.gfxdefs_path) };

		if(cfg.chr_outfile.empty() && cfg.pal_outfile.empty())
		{
			throw std::invalid_argument("No tile or palette output; nothing to do!");
		}

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

		chrdef chrdef { i_find_chrdef->second };

		// sanity check - coldef not required - if not specified, use default
		// built-in
		if(coldef_id.empty())
			coldef_id = "basic_8bit_random";

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

		auto i_find_paldef { gfxdefs.paldefs.find(paldef_id) };
		if(i_find_paldef == gfxdefs.paldefs.end())
		{
			std::cerr << "Could not find specified paldef" << std::endl;
			return -11;
		}

		paldef paldef { i_find_paldef->second };

#ifdef DEBUG
		std::chrono::high_resolution_clock::time_point t2 =
				std::chrono::high_resolution_clock::now();
		auto duration =
				std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count();

		std::cerr << "SETUP: " << duration << "ms" << std::endl;
		std::cerr << "  Using gfxdefs file: " << cfg.gfxdefs_path << std::endl;
		std::cerr << "  Using chrdef '" << chrdef_id << "'" << std::endl;
		std::cerr << "  Using colrdef '" << coldef_id << "'" << std::endl;
		std::cerr << "  Using paldef '" << paldef_id << "'" << std::endl;
#endif

#ifdef DEBUG
		t1 = std::chrono::high_resolution_clock::now();
#endif
		png::image<png::index_pixel> in_img(
				png_data, png::require_color_space<png::index_pixel>());

#ifdef DEBUG
		t2 = std::chrono::high_resolution_clock::now();
		duration =
				std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count();

		std::cerr << "LOAD PNG: " << std::to_string(duration) << "ms" << std::endl;
#endif

#ifdef DEBUG
		t1 = std::chrono::high_resolution_clock::now();
#endif

		// deal with tiles first
		if(!cfg.chr_outfile.empty())
		{
			buffer<byte_t> png_chunk { chrgfx::png_chunk(
					chrdef.width(), chrdef.height(), in_img.get_pixbuf()) };

#ifdef DEBUG
			t2 = std::chrono::high_resolution_clock::now();
			duration = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1)
										 .count();

			std::cerr << "PNG CHUNK: " << std::to_string(duration) << "ms"
								<< std::endl;
#endif

#ifdef DEBUG
			t1 = std::chrono::high_resolution_clock::now();
#endif
			std::ofstream chr_outfile { cfg.chr_outfile };
			if(!chr_outfile.good())
			{
				std::cerr << "chr-output error: " << std::strerror(errno) << std::endl;
				return -12;
			}

			size_t chunksize { (unsigned)(chrdef.datasize() / 8) };

			auto ptr_imgdata = png_chunk.data();
			auto ptr_imgdata_end = png_chunk.data() + png_chunk.datasize();

			while(ptr_imgdata != ptr_imgdata_end)
			{
				byte_t * temp_chr { encode_chr(chrdef, ptr_imgdata) };
				std::copy(temp_chr, temp_chr + chunksize,
									std::ostream_iterator<byte_t>(chr_outfile));
				ptr_imgdata += chrdef.width() * chrdef.height();
				delete[] temp_chr;
			}
		}
#ifdef DEBUG
		t2 = std::chrono::high_resolution_clock::now();
		duration =
				std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count();

		std::cerr << "CHR ENCODE & OUTPUT TO STREAM: " << std::to_string(duration)
							<< "ms" << std::endl;
#endif

		// deal with the palette next
		if(!cfg.pal_outfile.empty())
		{

#ifdef DEBUG
			t1 = std::chrono::high_resolution_clock::now();
#endif

			auto paldef_palette_data { encode_pal(paldef, *coldef,
																						in_img.get_palette()) };

#ifdef DEBUG
			t2 = std::chrono::high_resolution_clock::now();
			duration = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1)
										 .count();

			std::cerr << "CONVERT PALETTE: " << std::to_string(duration) << "ms"
								<< std::endl;
#endif

#ifdef DEBUG
			t1 = std::chrono::high_resolution_clock::now();
#endif

			std::ofstream pal_outfile { cfg.pal_outfile };
			if(!pal_outfile.good())
			{
				std::cerr << "pal-output error: " << std::strerror(errno) << std::endl;
			}

			// TODO: consider splitting the palette conversion routine into two
			// functions, on for subpal and one for full pal so we always know the
			// size of the data returned
			size_t filesize { (size_t)(paldef.datasize() / 8) };

			pal_outfile.write((char *)(paldef_palette_data), filesize);

#ifdef DEBUG
			t2 = std::chrono::high_resolution_clock::now();
			duration = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1)
										 .count();

			std::cerr << "PAL OUTPUT TO STREAM: " << std::to_string(duration) << "ms"
								<< std::endl;
#endif
		}

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

			// help
			case 'h':
				show_usage(long_opts.data(), opt_details.data());
				return true;
		}
	}
	return false;
}

void print_help()
{
	show_usage(long_opts.data(), opt_details.data());
}

/*
void print_help()
{
	std::cerr << APP::NAME << " - ver " << APP::VERSION << std::endl << std::endl;
	std::cerr << "Valid options:" << std::endl;
	std::cerr << "  --gfx-def, -G   Specify filename with "
							 "tile/palette/color/profile definitions"
						<< std::endl;
	std::cerr << "  --profile, -P   Specify graphics profile to use" << std::endl;
	std::cerr << "  --chr-def, -P   Specify tile format to use (overrides tile "
							 "settings in profile)"
						<< std::endl;
	std::cerr << "  --col-def, -P   Specify color format to use (overrides color "
							 "settings in profile)"
						<< std::endl;
	std::cerr << "  --pal-def, -P   Specify palette format to use (overrides "
							 "palette settings in profile)"
						<< std::endl;
	std::cerr
			<< "  --chr-def, -T   Specify tile data format (overrides tile format "
				 "in gfx-def)"
			<< std::endl;
	std::cerr << "  --png-data, -b     Filepath to input graphics data (PNG file)"
						<< std::endl;
	std::cerr << "  --chr-output, -c   Filepath to output tile data; if not "
							 "specified, no tile data will be generated"
						<< std::endl;
	std::cerr << "  --pal-output, -b   Filepath to output palette data; if not "
							 "specified, no palette data will be generated"
						<< std::endl;
	std::cerr << "  --subpalette, -s   Specify subpalette index; if not "
							 "specified, the full palette is used"
						<< std::endl;
	std::cerr << "  --help, -h         Display this text" << std::endl;
}
*/
