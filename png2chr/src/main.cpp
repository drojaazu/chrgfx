
#include "chrgfx.hpp"
#include "import_defs.hpp"
#include "shared.hpp"

#include <getopt.h>
#include <iostream>
#include <stdio.h>

#ifdef DEBUG
#include <chrono>
#endif

using namespace std;
using namespace chrgfx;

void process_args(int argc, char **argv);
void print_help();

struct runtime_config_png2chr : runtime_config {
	std::string pngdata_name{""};
};

// option settings
runtime_config_png2chr cfg;

int main(int argc, char **argv)
{
	istream *pngdata{nullptr};

	chr_def *chrdef{nullptr};
	pal_def *paldef{nullptr};

	// Runtime State Setup
	process_args(argc, argv);
	/*
		map<string, chr_def> chrdefs{load_gfxdefs("temp")};

		// a chrdef is always required
		// check for --gfx-def first
		if(!cfg.gfxdef_name.empty()) {
			ifstream gfxdef_file(cfg.gfxdef_name);
			if(gfxdef_file.fail()) {
				throw invalid_argument("Unable to open gfxdef file");
			}
			// only bother importing if chrdef/paldef are not defined (since they
			// which will override gfxdef in the next section)
			if(cfg.chrdef_name.empty()) {
				chrdef = load_chrdef(gfxdef_file);
			}
			// even if paldef is set, if paldata is null, don't worry about
			// the paldef import
			if(cfg.paldef_name.empty() & paldata != nullptr) {
				gfxdef_file.clear();
				paldef = load_paldef(gfxdef_file);
			}
		}

		// use individual chrdef/paldef settings, which will override
		// anything in the gfxdef
		if(!cfg.chrdef_name.empty()) {
			ifstream chrdef_file(cfg.chrdef_name);
			if(chrdef_file.fail()) {
				throw invalid_argument("Unable to open gfxdef file in chr-def option");
			}
			// delete chrdef in case it was previously created with gfxdef option
			delete chrdef;
			chrdef = load_chrdef(chrdef_file);
			if(chrdef == nullptr) {
				throw invalid_argument("Invalid chrdef file");
			}
		}

		if(chrdef == nullptr) {
			throw invalid_argument("No chrdef has been specified!");
		}

		if(!cfg.pngdata_name.empty()) {
			pngdata = new ifstream(cfg.pngdata_name);
			if(pngdata->fail()) {
				throw invalid_argument("PNG file could not be opened");
			}
		} else {
			// use cin as input
			pngdata = &cin;
		}

		try {
			png::image<png::index_pixel> in_img(
					cfg.pngdata_name, png::require_color_space<png::index_pixel>());
			bank *png_chrs = pngchunk(in_img, *chrdef);
			ostream *out(nullptr);
			if(cfg.outfile.empty())
				out = &cout;
			else {
				out = new ofstream(cfg.outfile);
			}

			for(size_t i = 0; i < png_chrs->chrs->size(); ++i) {
				defchr chrdata = chrgfx::to_defchr(*chrdef, png_chrs->chrs->at(i));
				out->write((char *)chrdata, chrdef->get_datasize() / 8);
				delete chrdata;
			}
			out->flush();
			delete png_chrs;

		} catch(const png::error &e) {
			cerr << "PNG error: " << e.what() << endl;
			return -10;
		}

		*/
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
	// cerr << "chrgfx version " << version << endl << endl;
	cerr << "Valid options:" << endl;
	cerr << "  --gfx-def, -G   Specify graphics data format" << endl;
	cerr << "  --chr-def, -C   Specify tile data format (overrides tile format "
					"in gfx-def)"
			 << endl;
	cerr << "  --chr-data, -c     Filename to input tile data" << endl;
	cerr << "  --pal-def, -P   Specify palette data format (overrides palette "
					"format in gfx-def)"
			 << endl;
	cerr << "  --pal-data, -p     Filename to input palette data" << endl;
	cerr << "  --output, -o       Specify output PNG image filename" << endl;
	cerr << "  --trns, -t         Use image transparency" << endl;
	cerr << "  --trns-index, -i   Specify palette entry to use as transparency "
					"(default is 0)"
			 << endl;
	cerr << "  --columns, -c      Specify number of columns per row of tiles in "
					"output image"
			 << endl;
	cerr << "  --subpalette, -s   Specify subpalette (default is 0)" << endl;
	cerr << "  --help, -h         Display this text" << endl;
}
