#include "app.hpp"
#include "chrgfx.hpp"
#include "import_defs.hpp"
#include "shared.hpp"

#include <cerrno>
#include <getopt.h>
#include <iostream>
#include <map>
#include <stdio.h>

using std::map;
using std::string;
using std::vector;
using namespace chrgfx;

#ifdef DEBUG
#include <chrono>
#endif

bool process_args(int argc, char ** argv);
void print_help();

struct runtime_config_chr2png : runtime_config
{
	string chrdata_name { "" }, paldata_name { "" };
	chrgfx::render_traits rendertraits;
	string outfile { "" };
};

// option settings
runtime_config_chr2png cfg;

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
			}
		}
		catch(std::exception const & e)
		{
			// failure in argument parsing/runtime config
			std::cerr << "Invalid argument: " << e.what() << std::endl;
			return -5;
		}

		// see if we even have good input before moving on
		std::ifstream chrdata { cfg.chrdata_name };
		if(!chrdata.good())
		{
			std::cerr << "chr-data error: " << std::strerror(errno) << std::endl;
			return -6;
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
			coldef = (chrgfx::coldef *)&(rgbcoldef_iter->second);
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
		std::cerr << "Using chrdef '" << chrdef_id << "'" << std::endl;
		std::cerr << "Using colrdef '" << coldef_id << "'" << std::endl;
		std::cerr << "Using paldef '" << paldef_id << "'" << std::endl;
#endif

#ifdef DEBUG
		t1 = std::chrono::high_resolution_clock::now();
#endif

		// tiles first
		vector<uptr<u8>> workbank;
		size_t chunksize = (chrdef.datasize() / 8);
		char chunkbuffer[chunksize];
		buffer<byte_t> workbuffer(0);
		size_t outchunk_size = chrdef.width() * chrdef.height();

		while(1)
		{
			chrdata.read(chunkbuffer, chunksize);
			if(chrdata.eof())
				break;

			workbuffer.append(decode_chr(chrdef, chunkbuffer), outchunk_size);

			// workbank.push_back(uptr<u8>(to_basic_chr(chrdef, (u8 *)chunkbuffer)));
		}

#ifdef DEBUG
		t2 = std::chrono::high_resolution_clock::now();
		duration =
				std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count();

		std::cerr << "TILE CONVERSION: " << std::to_string(duration) << "ms"
							<< std::endl;
#endif

#ifdef DEBUG
		t1 = std::chrono::high_resolution_clock::now();
#endif

		// then the palette
		palette workpal;
		if(cfg.paldata_name != "")
		{
			std::ifstream paldata { cfg.paldata_name };
			if(!paldata.good())
			{
				std::cerr << "pal-data error: " << std::strerror(errno) << std::endl;
				return -12;
			}

			// get the size of the palette data
			paldata.seekg(0, paldata.end);
			long length { paldata.tellg() };

			// TODO: work on a less grody way to do checks on palette data size
			// should probably develop a wrapper around the data with size, etc

			// if we haven't specified a subpalette, but we don't have enough data for
			// the full palette, specify a subpalette anyway so we don't barf out junk
			// data into the color palette
			if(!cfg.subpalette)
			{
				cfg.subpalette = 0;
			}

			if(!cfg.subpalette && length < (paldef.datasize() / 8))
			{
				throw std::invalid_argument("Not enough palette data available");
			}

			if(cfg.subpalette &&
				 ((cfg.subpalette.value() * (paldef.datasize() / 8)) > length))
			{
				throw std::out_of_range("Specified subpalette index is outside the "
																"size of the supplied palette data");
			}

			paldata.seekg(0, paldata.beg);
			byte_t palbuffer[length];
			paldata.read(palbuffer, length);

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
				chrdef, workbuffer, workpal, cfg.rendertraits) };

#ifdef DEBUG
		t2 = std::chrono::high_resolution_clock::now();
		duration =
				std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count();
		std::cerr << "PNG RENDER: " << duration << "ms" << std::endl;
#endif

#ifdef DEBUG
		t1 = std::chrono::high_resolution_clock::now();
#endif

		if(cfg.outfile.empty())
		{
			outimg.write_stream(std::cout);
		}
		else
		{
			outimg.write(cfg.outfile);
		}
#ifdef DEBUG
		t2 = std::chrono::high_resolution_clock::now();
		duration =
				std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count();
		std::cerr << "OUTPUT: " << duration << "ms" << std::endl;
#endif

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
	default_long_opts.push_back({ "trns", no_argument, nullptr, 't' });
	default_long_opts.push_back(
			{ "trns-index", required_argument, nullptr, 'i' });
	default_long_opts.push_back({ "border", no_argument, nullptr, 'b' });
	default_long_opts.push_back({ "columns", required_argument, nullptr, 'd' });
	default_long_opts.push_back({ "chr-data", required_argument, nullptr, 'c' });
	default_long_opts.push_back({ "pal-data", required_argument, nullptr, 'p' });
	default_long_opts.push_back(
			{ "subpalette", required_argument, nullptr, 's' });
	default_long_opts.push_back({ "output", required_argument, nullptr, 'o' });
	default_long_opts.push_back({ "help", no_argument, nullptr, 'h' });
	default_short_opts.append("ti:bd:c:p:s:o:h");

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
				// chr-data
			case 'c':
				cfg.chrdata_name = optarg;
				break;

			// pal-data
			case 'p':
				cfg.paldata_name = optarg;
				break;

			// subpalette
			case 's':
				try
				{
					cfg.subpalette = std::stoi(optarg);
				}
				catch(const std::invalid_argument & e)
				{
					throw std::invalid_argument("Invalid subpalette index value");
				}
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
					cfg.rendertraits.columns = std::stoi(optarg);
				}
				catch(const std::invalid_argument & e)
				{
					throw std::invalid_argument("Invalid columns value");
				}
				break;

			// output
			case 'o':
				cfg.outfile = optarg;
				break;

			case 'h':
				print_help();
				return true;
		}
	}

	return false;
}

void print_help()
{
	std::cout << APP::NAME << " - ver " << APP::VERSION << std::endl << std::endl;
	std::cout << "Valid options:" << std::endl;
	std::cout << "  --gfx-def, -G   Specify graphics data format" << std::endl;
	std::cout
			<< "  --chr-def, -C   Specify tile data format (overrides tile format "
				 "in gfx-def)"
			<< std::endl;
	std::cout << "  --chr-data, -c     Filename to input tile data" << std::endl;
	std::cout
			<< "  --pal-def, -P   Specify palette data format (overrides palette "
				 "format in gfx-def)"
			<< std::endl;
	std::cout << "  --pal-data, -p     Filename to input palette data"
						<< std::endl;
	std::cout << "  --output, -o       Specify output PNG image filename"
						<< std::endl;
	std::cout << "  --trns, -t         Use image transparency" << std::endl;
	std::cout << "  --border, -b       Draw 1 pixel border around tiles"
						<< std::endl;
	std::cout
			<< "  --trns-index, -i   Specify palette entry to use as transparency "
				 "(default is 0)"
			<< std::endl;
	std::cout
			<< "  --columns, -c      Specify number of columns per row of tiles in "
				 "output image"
			<< std::endl;
	std::cout << "  --subpalette, -s   Specify subpalette (default is 0)"
						<< std::endl;
	std::cout << "  --help, -h         Display this text" << std::endl;
}
