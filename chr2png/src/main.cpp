//#include "main.hpp"
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

// application globals
const static string version = string("1.1");
int return_status;

struct runtime_config_chr2png : runtime_config {
	std::string chrdata_name{""}, paldata_name{""};
	chrgfx::render_traits rendertraits;
};

// option settings
runtime_config_chr2png cfg;

int main(int argc, char **argv)
{

	// declare our heap variables here in case they need
	// to be deleted from exception
	istream *chrdata{nullptr};
	istream *paldata{nullptr};
	// istream *pngdata{nullptr};

	chr_def *chrdef{nullptr};
	pal_def *paldef{nullptr};

	palette workpal;

	try {

		// Runtime State Setup
		process_args(argc, argv);

		std::map<std::string, chrgfx::chr_def> chrdefs{
				load_chrdefs(cfg.gfxdef_name)};
		// std::map<std::string, chrgfx::col_def> coldefs;
		// std::map<std::string, chrgfx::pal_def> paldefs;

		////////////////
		// chrdata is not required; if not present, use cin
		if(!cfg.chrdata_name.empty()) {
			chrdata = new ifstream(cfg.chrdata_name);
			if(chrdata->fail()) {
				throw invalid_argument("Tile data file could not be opened");
			}
		} else {
			// use cin as input
			chrdata = &cin;
		}

		// paldata is not required; if not present, use system generated colors
		if(cfg.paldata_name.empty()) {
			paldata = nullptr;
		} else {
			paldata = new ifstream(cfg.paldata_name);
			if(paldata->fail()) {
				throw invalid_argument("Palette data file could not be opened");
			}
		}

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

		// if paldata is null, we don't even bother with the paldef
		if(paldata != nullptr && !cfg.paldef_name.empty()) {
			ifstream paldef_file(cfg.paldef_name);
			if(paldef_file.fail()) {
				throw invalid_argument("Unable to open gfxdef file in pal-def option");
			}
			// delete paldef in case it was previously created with gfxdef option
			delete paldef;
			paldef = load_paldef(paldef_file);
			if(paldef == nullptr) {
				throw invalid_argument("Invalid paldef file");
			}
		}

		if(paldef == nullptr) {
			// create a generic palette if no palette data supplied

			workpal = chrgfx::make_pal(false);
		} else {
			paldata->seekg(0, paldata->end);
			int length = paldata->tellg();
			paldata->seekg(0, paldata->beg);

			char palbuffer[length];
			paldata->read(palbuffer, length);
			workpal = chrgfx::to_rawpal(*paldef, (u8 *)palbuffer, cfg.subpalette);
		}
	} catch(const exception &e) {
		// failure in argument parsing/runtime config
		cerr << e.what() << endl;
		return -1;
	}

#ifdef DEBUG
	std::chrono::high_resolution_clock::time_point t1 =
			std::chrono::high_resolution_clock::now();
#endif

	bank workbank = bank(*chrdef);
	// read the file in chunks, convert each chunk and store it in a vector

	auto chunksize = (chrdef->get_datasize() / 8);

	char chunkbuffer[chunksize];

	while(1) {
		chrdata->read(chunkbuffer, chunksize);
		if(chrdata->eof())
			break;

		workbank.chrs->push_back(chrgfx::to_rawchr(*chrdef, (u8 *)chunkbuffer));
	}

#ifdef DEBUG
	std::chrono::high_resolution_clock::time_point t2 =
			std::chrono::high_resolution_clock::now();
	auto duration =
			std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count();

	cerr << "Tile conversion: " << duration << " ticks" << endl;

	t1 = std::chrono::high_resolution_clock::now();
#endif
	png::image<png::index_pixel> *outimg =
			render(workbank, workpal, cfg.rendertraits);
	try {

#ifdef DEBUG
		t2 = std::chrono::high_resolution_clock::now();
		duration =
				std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count();
		cerr << "Rendering: " << duration << " ticks" << endl;
#endif

		if(cfg.outfile.empty())
			outimg->write_stream(cout);
		else
			outimg->write(cfg.outfile);

		delete outimg;
	} catch(const exception &e) {
		delete outimg;
		throw e;
	}
	return_status = 0;
	goto cleanup;

cleanup:
	if(chrdata != &cin) {
		delete chrdata;
	}
	delete paldata;

	delete chrdef;
	if(paldef != nullptr) {
		delete paldef->get_coldef();
		delete paldef->get_syspal();
	}
	delete paldef;
	// delete workpal;

	return return_status;
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
		}
	}
}

void print_help()
{
	cerr << "chrgfx version " << version << endl << endl;
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
