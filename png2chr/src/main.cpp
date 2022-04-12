
#include "app.hpp"
#include "chrgfx.hpp"
#include "import_defs.hpp"
#include "shared.hpp"

#include <cerrno>
#include <getopt.h>
#include <iostream>
#include <iterator>
#include <stdio.h>

#ifdef DEBUG
#include <chrono>
#endif

using std::map;
using std::string;
using std::vector;
using namespace chrgfx;

bool process_args(int argc, char ** argv);
void print_help();

struct runtime_config_png2chr : runtime_config
{
	string pngdata_name { "" };
	string chr_outfile { "" };
	string pal_outfile { "" };
};

// option settings
runtime_config_png2chr cfg;

struct coldef
{
	rgbcoldef const * p_rgbcoldef { nullptr };
	refcoldef const * p_refcoldef { nullptr };
};

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
		std::ifstream png_file;
		std::istream & png_data = (cfg.pngdata_name.empty() ? std::cin : png_file);
		if(!cfg.pngdata_name.empty())
		{
			png_file.open(cfg.pngdata_name);
			if(!png_file.good())
			{
				std::cerr << "png-data error: " << std::strerror(errno) << std::endl;
				return -6;
			}
		}

		// load definitions
		std::tuple<
				map<string const, chrdef const>, map<string const, rgbcoldef const>,
				map<string const, refcoldef const>, map<string const, paldef const>,
				map<string const, gfxprofile const>>
				defs;

		try
		{
			defs = load_gfxdefs(cfg.gfxdefs_path);
		}
		catch(std::ios_base::failure const & e)
		{
			std::cerr << "gfx-def error: " << std::strerror(errno) << std::endl;
			return -7;
		}

		map<string const, chrdef const> chrdefs { std::get<0>(defs) };
		map<string const, rgbcoldef const> rgbcoldefs { std::get<1>(defs) };
		map<string const, refcoldef const> refcoldefs { std::get<2>(defs) };
		map<string const, paldef const> paldefs { std::get<3>(defs) };
		map<string const, gfxprofile const> profiles { std::get<4>(defs) };

		if(cfg.chr_outfile.empty() && cfg.pal_outfile.empty())
		{
			throw std::invalid_argument("No tile or palette output; nothing to do!");
		}

		string chrdef_id, coldef_id, paldef_id;

		auto profile_iter { profiles.find(cfg.profile) };
		if(profile_iter != profiles.end())
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

		auto chrdef_iter { chrdefs.find(chrdef_id) };
		if(chrdef_iter == chrdefs.end())
		{
			std::cerr << "Could not find specified chrdef" << std::endl;
			return -9;
		}

		chrdef chrdef { chrdef_iter->second };

		// sanity check - coldef not required - if not specified, use default
		// built-in
		if(coldef_id.empty())
		{
			coldef_id = "basic_8bit_random";
		}

		chrgfx::coldef * coldef;

		auto rgbcoldef_iter { rgbcoldefs.find(coldef_id) };
		if(rgbcoldef_iter != rgbcoldefs.end())
		{
			coldef = (chrgfx::coldef *)&rgbcoldef_iter->second;
		}
		else
		{
			auto refcoldef_iter { refcoldefs.find(coldef_id) };
			if(refcoldef_iter != refcoldefs.end())
			{
				coldef = (chrgfx::coldef *)&refcoldef_iter->second;
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

		auto paldef_iter { paldefs.find(paldef_id) };
		if(paldef_iter == paldefs.end())
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
		std::cerr << "Using gfxdefs file: " << cfg.gfxdefs_path << std::endl;
		std::cerr << "Using chrdef '" << chrdef_id << "'" << std::endl;
		std::cerr << "Using colrdef '" << coldef_id << "'" << std::endl;
		std::cerr << "Using paldef '" << paldef_id << "'" << std::endl;
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
			buffer<byte_t> png_chunk { chrgfx::png_chunk(chrdef,
																									 in_img.get_pixbuf()) };

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

			auto iter = png_chunk.begin();
			auto iter_end = png_chunk.end();

			while(iter != iter_end)
			{
				byte_t * temp_chr { encode_chr(chrdef, &(*iter)) };
				std::copy(temp_chr, temp_chr + chunksize,
									std::ostream_iterator<u8>(chr_outfile));
				iter += chrdef.datasize() / 8;
			}
		}
#ifdef DEBUG
		t2 = std::chrono::high_resolution_clock::now();
		duration =
				std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count();

		std::cerr << "CHR OUTPUT: " << std::to_string(duration) << "ms"
							<< std::endl;
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
			size_t filesize { paldef.datasize() / 8 };

			pal_outfile.write((char *)(paldef_palette_data.get()), filesize);

#ifdef DEBUG
			t2 = std::chrono::high_resolution_clock::now();
			duration = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1)
										 .count();

			std::cerr << "PAL OUTPUT: " << std::to_string(duration) << "ms"
								<< std::endl;
#endif
		}

		// everything's good, we're outta here
		return 0;
	}
	catch(std::exception const & e)
	{
		std::cerr << "FATAL EXCEPTION: " << e.what() << std::endl;
		return -1;
	}
}

bool process_args(int argc, char ** argv)
{
	default_long_opts.push_back(
			{ "chr-output", required_argument, nullptr, 'c' });
	default_long_opts.push_back(
			{ "pal-output", required_argument, nullptr, 'p' });
	default_long_opts.push_back({ "png-data", required_argument, nullptr, 'b' });
	default_long_opts.push_back({ "help", no_argument, nullptr, 'h' });
	default_short_opts.append("c:p:b:h");

	bool default_processed = process_default_args(cfg, argc, argv);

	if(!default_processed)
	{
		print_help();
		exit(1);
	}

	while(true)
	{
		const auto this_opt = getopt_long(argc, argv, default_short_opts.data(),
																			default_long_opts.data(), nullptr);
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
				print_help();
				return true;
		}
	}
	return false;
}

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
