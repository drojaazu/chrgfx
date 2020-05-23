
#include "chrgfx.hpp"
#include "import_defs.hpp"
#include "shared.hpp"

#include <cerrno>
#include <getopt.h>
#include <iostream>
#include <stdio.h>

#ifdef DEBUG
#include <chrono>
#endif

using std::string;
using std::vector;
using namespace chrgfx;

void process_args(int argc, char **argv);
void print_help();

struct runtime_config_png2chr : runtime_config {
	string pngdata_name{""};
	string chr_outfile{""};
	string pal_outfile{""};
};

// option settings
runtime_config_png2chr cfg;

int main(int argc, char **argv)
{
	try {
		// Runtime State Setup
		process_args(argc, argv);
	} catch(std::exception const &e) {
		std::cerr << e.what() << std::endl;
		return -1;
	}

	std::ifstream pngdata{cfg.pngdata_name};
	if(!pngdata.good()) {
		throw std::ios_base::failure(std::strerror(errno));
	}

	/////////// setup definitions

	auto defs = load_gfxdefs(cfg.gfxdef);

	map<string const, chrdef const> chrdefs{std::get<0>(defs)};
	map<string const, coldef const> coldefs{std::get<1>(defs)};
	map<string const, paldef const> paldefs{std::get<2>(defs)};
	map<string const, gfxprofile const> profiles{std::get<3>(defs)};

	auto profile_iter{profiles.find(cfg.profile)};
	if(profile_iter == profiles.end()) {
		throw "Could not find specified gfxprofile";
	}

	gfxprofile profile{profile_iter->second};

	auto chrdef_iter{chrdefs.find(profile.get_chrdef_id())};
	if(chrdef_iter == chrdefs.end()) {
		throw "Could not find specified chrdef";
	}

	chrdef chrdef{chrdef_iter->second};

	auto coldef_iter{coldefs.find(profile.get_coldef_id())};
	if(coldef_iter == coldefs.end()) {
		throw "Could not find specified coldef";
	}

	coldef coldef{coldef_iter->second};

	auto paldef_iter{paldefs.find(profile.get_paldef_id())};
	if(paldef_iter == paldefs.end()) {
		throw "Could not find specified paldef";
	}

	paldef paldef{paldef_iter->second};

	try {
		png::image<png::index_pixel> in_img(
				cfg.pngdata_name, png::require_color_space<png::index_pixel>());

#ifdef DEBUG
		std::chrono::high_resolution_clock::time_point t1 =
				std::chrono::high_resolution_clock::now();
#endif

		chrbank png_chrbank{chrgfx::pngchunk(chrdef, in_img.get_pixbuf())};

#ifdef DEBUG
		std::chrono::high_resolution_clock::time_point t2 =
				std::chrono::high_resolution_clock::now();
		auto duration =
				std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count();

		std::cerr << "Tile conversion: " << duration << "ms" << std::endl;

#endif

		std::ofstream chr_outfile{cfg.chr_outfile};
		if(!chr_outfile.good()) {
			throw std::ios_base::failure(std::strerror(errno));
		}

		size_t chunksize{chrdef.get_datasize() / 8};

		for(const auto &chr : png_chrbank) {
			std::copy(chr, chr + chunksize, std::ostream_iterator<u8>(chr_outfile));
		}

		// BIG TODO: to non-raw palette routine

	} catch(const png::error &e) {
		std::cerr << "PNG error: " << e.what() << std::endl;
		return -10;
	}

	return 0;
}

void process_args(int argc, char **argv)
{
	default_long_opts.push_back({"trns", no_argument, nullptr, 't'});
	default_long_opts.push_back({"trns-index", required_argument, nullptr, 'i'});
	default_long_opts.push_back({"columns", required_argument, nullptr, 'd'});
	default_long_opts.push_back({"chr-data", required_argument, nullptr, 'c'});
	default_long_opts.push_back({"pal-data", required_argument, nullptr, 'p'});
	default_short_opts.append("ti:d:c:p:");

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
			case 'p':
				cfg.pngdata_name = optarg;
				break;
		}
	}
}

void print_help()
{
	// std::cerr << "chrgfx version " << version << std::endl << std::endl;
	std::cerr << "Valid options:" << std::endl;
	std::cerr << "  --gfx-def, -G   Specify graphics data format" << std::endl;
	std::cerr
			<< "  --chr-def, -C   Specify tile data format (overrides tile format "
				 "in gfx-def)"
			<< std::endl;
	std::cerr << "  --chr-data, -c     Filename to input tile data" << std::endl;
	std::cerr
			<< "  --pal-def, -P   Specify palette data format (overrides palette "
				 "format in gfx-def)"
			<< std::endl;
	std::cerr << "  --pal-data, -p     Filename to input palette data"
						<< std::endl;
	std::cerr << "  --output, -o       Specify output PNG image filename"
						<< std::endl;
	std::cerr << "  --trns, -t         Use image transparency" << std::endl;
	std::cerr
			<< "  --trns-index, -i   Specify palette entry to use as transparency "
				 "(default is 0)"
			<< std::endl;
	std::cerr
			<< "  --columns, -c      Specify number of columns per row of tiles in "
				 "output image"
			<< std::endl;
	std::cerr << "  --subpalette, -s   Specify subpalette (default is 0)"
						<< std::endl;
	std::cerr << "  --help, -h         Display this text" << std::endl;
}
