#include "chrgfx.hpp"
#include "import_defs.hpp"
#include "shared.hpp"

#include <bits/stdc++.h>
#include <cerrno>
#include <getopt.h>
#include <iostream>
#include <map>
#include <stdio.h>

using std::string;
using std::vector;
using namespace chrgfx;

#ifdef DEBUG
#include <chrono>
#endif

void process_args(int argc, char **argv);
void print_help();

// application globals
const static string version = string("1.1");

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
		// Runtime State Setup
		process_args(argc, argv);
	} catch(std::exception const &e) {
		// failure in argument parsing/runtime config
		std::cerr << e.what() << std::endl;
		return -1;
	}

	std::ifstream chrdata{cfg.chrdata_name};
	if(!chrdata.good()) {
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

#ifdef DEBUG
	std::chrono::high_resolution_clock::time_point t1 =
			std::chrono::high_resolution_clock::now();
#endif

	chrbank workbank{chrdef};
	auto chunksize = (chrdef.get_datasize() / 8);
	char chunkbuffer[chunksize];

	while(1) {
		chrdata.read(chunkbuffer, chunksize);
		if(chrdata.eof())
			break;

		workbank.push_back(chrgfx::from_chr(chrdef, (u8 *)chunkbuffer));
	}

	palette workpal;
	if(cfg.paldata_name != "") {
		std::ifstream paldata{cfg.paldata_name};
		if(!paldata.good()) {
			throw std::ios_base::failure("Could not open palette data file - " +
																	 (string)std::strerror(errno));
		}

		paldata.seekg(0, paldata.end);
		int length = paldata.tellg();
		paldata.seekg(0, paldata.beg);

		char palbuffer[length];
		paldata.read(palbuffer, length);
		workpal = from_pal(paldef, coldef, (u8 *)palbuffer, cfg.subpalette);

	} else {
		workpal = make_pal_random();
	}

#ifdef DEBUG
	std::chrono::high_resolution_clock::time_point t2 =
			std::chrono::high_resolution_clock::now();
	auto duration =
			std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count();

	std::cerr << "Tile conversion: " << duration << "ms" << std::endl;

	t1 = std::chrono::high_resolution_clock::now();
#endif
	png::image<png::index_pixel> outimg{
			render(workbank, workpal, cfg.rendertraits)};

#ifdef DEBUG
	t2 = std::chrono::high_resolution_clock::now();
	duration =
			std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count();
	std::cerr << "Rendering: " << duration << "ms" << std::endl;
#endif

	if(cfg.outfile.empty()) {
		outimg.write_stream(std::cout);
	} else {
		outimg.write(cfg.outfile);
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
	default_long_opts.push_back({"output", required_argument, nullptr, 'o'});
	default_short_opts.append("ti:d:c:p:o:");

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
		}
	}
}

void print_help()
{
	std::cerr << "chrgfx version " << version << std::endl << std::endl;
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