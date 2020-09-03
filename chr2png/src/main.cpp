#include "chrgfx.hpp"
#include "import_defs.hpp"
#include "shared.hpp"

#include <bits/stdc++.h>
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

bool process_args(int argc, char **argv);
void print_help();

// application globals
static unsigned int const APP_VERSION_MAJOR{1};
static unsigned int const APP_VERSION_MINOR{0};
static unsigned int const APP_VERSION_FIX{0};
static std::string const APP_VERSION{std::to_string(APP_VERSION_MAJOR) + "." +
																		 std::to_string(APP_VERSION_MINOR) + "." +
																		 std::to_string(APP_VERSION_FIX)};
static std::string const APP_NAME{"chr2png"};

struct runtime_config_chr2png : runtime_config {
	string chrdata_name{""}, paldata_name{""};
	chrgfx::render_traits rendertraits;
	string outfile{""};
};

// option settings
runtime_config_chr2png cfg;

int main(int argc, char **argv)
{
	try {
#ifdef DEBUG
		std::chrono::high_resolution_clock::time_point t1 =
				std::chrono::high_resolution_clock::now();
#endif
		try {
			// Runtime State Setup
			if(process_args(argc, argv)) {
				return 0;
			}
		} catch(std::exception const &e) {
			// failure in argument parsing/runtime config
			std::cerr << "Invalid argument: " << e.what() << std::endl;
			return -5;
		}

		// see if we even have good input before moving on
		std::ifstream chrdata{cfg.chrdata_name};
		if(!chrdata.good()) {
			std::cerr << "chr-data error: " << std::strerror(errno) << std::endl;
			return -6;
		}

		// converter function pointers
		conv_chr::cvfrom_chr_t chr_from_converter;
		conv_color::cvfrom_col_t col_from_converter;
		conv_palette::cvfrom_pal_t pal_from_converter;

		// load definitions
		std::tuple<map<string const, chrdef const>, map<string const, coldef const>,
							 map<string const, paldef const>,
							 map<string const, gfxprofile const>>
				defs;

		try {
			defs = load_gfxdefs(cfg.gfxdefs_file);
		} catch(std::ios_base::failure const &e) {
			std::cerr << "gfx-def error: " << std::strerror(errno) << std::endl;
			return -7;
		}

		map<string const, chrdef const> chrdefs{std::get<0>(defs)};
		map<string const, coldef const> coldefs{std::get<1>(defs)};
		map<string const, paldef const> paldefs{std::get<2>(defs)};
		map<string const, gfxprofile const> profiles{std::get<3>(defs)};

		string chrdef_id, coldef_id, paldef_id;

		auto profile_iter{profiles.find(cfg.profile)};
		if(profile_iter != profiles.end()) {
			gfxprofile profile{profile_iter->second};
			chrdef_id = profile.get_chrdef_id();
			coldef_id = profile.get_coldef_id();
			paldef_id = profile.get_paldef_id();
		}

		// specific gfxdefs override profile settings
		if(!cfg.chrdef.empty()) {
			chrdef_id = cfg.chrdef;
		}
		if(!cfg.coldef.empty()) {
			coldef_id = cfg.coldef;
		}
		if(!cfg.paldef.empty()) {
			paldef_id = cfg.paldef;
		}

		// sanity check - chrdef is required
		if(chrdef_id.empty()) {
			std::cerr << "Must specify a tile definition (--chr-def or --profile)"
								<< std::endl;
			return -8;
		}

		auto chrdef_iter{chrdefs.find(chrdef_id)};
		if(chrdef_iter == chrdefs.end()) {
			std::cerr << "Could not find specified chrdef" << std::endl;
			return -9;
		}

		chrdef chrdef{chrdef_iter->second};

		// sanity check - coldef not required - if not specified, use default
		// built-in
		if(coldef_id.empty()) {
			coldef_id = "basic_8bit_random";
		}

		auto coldef_iter{coldefs.find(coldef_id)};
		if(coldef_iter == coldefs.end()) {
			std::cerr << "Could not find specified coldef" << std::endl;
			return -10;
		}

		coldef coldef{coldef_iter->second};

		// sanity check - paldef not required - if not specified, use default
		// built-in
		if(paldef_id.empty()) {
			paldef_id = "basic_256color";
		}

		auto paldef_iter{paldefs.find(paldef_id)};
		if(paldef_iter == paldefs.end()) {
			std::cerr << "Could not find specified paldef" << std::endl;
			return -11;
		}

		paldef paldef{paldef_iter->second};

		chr_from_converter = chrdef.get_converter_from();
		pal_from_converter = paldef.get_converter_from();
		col_from_converter = coldef.get_converter_from();

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
		chrbank workbank{chrdef};
		auto chunksize = (chrdef.get_datasize() / 8);
		char chunkbuffer[chunksize];

		while(1) {
			chrdata.read(chunkbuffer, chunksize);
			if(chrdata.eof())
				break;

			workbank.push_back(
					uptr<u8>(chr_from_converter(chrdef, (u8 *)chunkbuffer)));
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
		if(cfg.paldata_name != "") {
			std::ifstream paldata{cfg.paldata_name};
			if(!paldata.good()) {
				std::cerr << "pal-data error: " << std::strerror(errno) << std::endl;
				return -12;
			}

			paldata.seekg(0, paldata.end);
			long length{paldata.tellg()};
			paldata.seekg(0, paldata.beg);

			char palbuffer[length];
			paldata.read(palbuffer, length);
			// TODO: work on a less grody way to do this
			// if we haven't specified a subpalette, but we don't have enough data for
			// the full palette, specify a subpalette anyway so we don't barf out junk
			// data into the color palette
			if(!cfg.subpalette && length < paldef.get_palette_datasize_bytes()) {
				cfg.subpalette = 0;
			}

			if(!cfg.subpalette && length < paldef.get_subpal_datasize_bytes()) {
				throw std::invalid_argument("Not enough palette data available");
			}

			workpal = pal_from_converter(paldef, coldef, (u8 *)palbuffer,
																	 cfg.subpalette, col_from_converter);

		} else {
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

		png::image<png::index_pixel> outimg{
				png_render(workbank, workpal, cfg.rendertraits)};

#ifdef DEBUG
		t2 = std::chrono::high_resolution_clock::now();
		duration =
				std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count();
		std::cerr << "PNG RENDER: " << duration << "ms" << std::endl;
#endif

#ifdef DEBUG
		t1 = std::chrono::high_resolution_clock::now();
#endif

		if(cfg.outfile.empty()) {
			outimg.write_stream(std::cout);
		} else {
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
	} catch(std::exception const &e) {
		std::cerr << "FATAL EXCEPTION: " << e.what() << std::endl;
		return -1;
	}
}

bool process_args(int argc, char **argv)
{
	default_long_opts.push_back({"trns", no_argument, nullptr, 't'});
	default_long_opts.push_back({"trns-index", required_argument, nullptr, 'i'});
	default_long_opts.push_back({"border", no_argument, nullptr, 'b'});
	default_long_opts.push_back({"columns", required_argument, nullptr, 'd'});
	default_long_opts.push_back({"chr-data", required_argument, nullptr, 'c'});
	default_long_opts.push_back({"pal-data", required_argument, nullptr, 'p'});
	default_long_opts.push_back({"subpalette", required_argument, nullptr, 's'});
	default_long_opts.push_back({"output", required_argument, nullptr, 'o'});
	default_long_opts.push_back({"help", no_argument, nullptr, 'h'});
	default_short_opts.append("ti:bd:c:p:s:o:h");

	bool default_processed = process_default_args(cfg, argc, argv);

	if(!default_processed) {
		print_help();
		exit(1);
	}

	while(true) {
		const auto this_opt = getopt_long(argc, argv, default_short_opts.data(),
																			default_long_opts.data(), nullptr);
		if(this_opt == -1)
			break;

		switch(this_opt) {
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
				try {
					cfg.subpalette = std::stoi(optarg);
				} catch(const std::invalid_argument &e) {
					throw std::invalid_argument("Invalid subpalette index value");
				}
				break;

			// trns
			case 't':
				cfg.rendertraits.use_trns = true;
				break;

			// trns entry
			case 'i':
				try {
					cfg.rendertraits.trns_entry = std::stoi(optarg);
				} catch(const std::invalid_argument &e) {
					throw std::invalid_argument("Invalid transparency index value");
				}
				break;

			// draw tile border
			case 'b':
				cfg.rendertraits.draw_border = true;
				break;

			// columns
			case 'd':
				try {
					cfg.rendertraits.cols = std::stoi(optarg);
				} catch(const std::invalid_argument &e) {
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
	std::cout << APP_NAME << " - ver " << APP_VERSION << std::endl << std::endl;
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
