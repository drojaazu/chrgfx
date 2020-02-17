#include "main.hpp"

#ifdef DEBUG
#include <chrono>
#endif

using namespace std;
using namespace chrgfx;

// application globals
const static string version = string("1.1");
int return_status;

// option settings
runtime_config cfg;

int main(int argc, char **argv)
{
	// declare our heap variables here in case they need
	// to be deleted from exception
	istream *chrdata{nullptr};
	istream *paldata{nullptr};
	istream *pngdata{nullptr};

	chr_def *chrdef{nullptr};
	pal_def *paldef{nullptr};

	palette *workpal{nullptr};

	try {
		// PARSE ARGUMENTS
		process_args(argc, argv);

		// RUNTIME SETUP
		// we either use a PNG file or a combination of CHR and PAL data
		// from seperate streams for data input

		// chrdata is not required; if not present, use cin
		if(!cfg.chrdata_name.empty()) {
			// default mode is chr -> png, so if chr-data is present,
			// ignore the to-chr mode
			cfg.to_chr = false;
			chrdata = new ifstream(cfg.chrdata_name);
			if(chrdata->fail()) {
				throw invalid_argument("Tile data file could not be opened");
			}
		} else if(!cfg.pngdata_name.empty()) {
			// no chrdata, but we have pngdata
			cfg.to_chr = true;
			pngdata = new ifstream(cfg.pngdata_name);
			if(pngdata->fail()) {
				throw invalid_argument("PNG file could not be opened");
			}
		} else {
			// neither chrdata or pngdata were specified
			// use cin as input for either chr or png, depening on switch
			if(cfg.to_chr) {
				pngdata = &cin;
			} else {
				chrdata = &cin;
			}
		}

		// paldata is not required; if not present, use system generated colors
		// (and skip entirely if we're doing png -> chr)
		if(!cfg.to_chr) {
			if(cfg.paldata_name.empty()) {
				paldata = nullptr;
			} else {
				paldata = new ifstream(cfg.paldata_name);
				if(paldata->fail()) {
					throw invalid_argument("Palette data file could not be opened");
				}
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
			workpal = paldef->convert((u8 *)palbuffer, cfg.subpalette);
		}
	} catch(const exception &e) {
		// failure in argument parsing/runtime config
		cerr << e.what() << endl;
		return -1;
	}

	if(!cfg.to_chr) {
		// --------- CHR to PNG
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

			workbank.chrs->push_back(chrdef->convert((u8 *)chunkbuffer));
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
				render(workbank, *workpal, cfg.rendertraits);
		try {

#ifdef DEBUG
			t2 = std::chrono::high_resolution_clock::now();
			duration = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1)
										 .count();
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
		delete workpal;
	} else {
		// ------- PNG to CHR
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
				bptr chrdata = to_chr(*chrdef, png_chrs->chrs->at(i));
				out->write((char *)chrdata, chrdef->get_datasize() / 8);
				delete chrdata;
			}
			out->flush();
			delete png_chrs;

		} catch(const png::error &e) {
			cerr << "PNG error: " << e.what() << endl;
			return -10;
		}
	}

	return return_status;
}

void process_args(int argc, char **argv)
{
	const char *const shortOpts = ":f:g:t:p:o:rc:s:h";
	const option longOpts[] = {{"gfx-def", required_argument, nullptr, 'G'},
														 {"chr-def", required_argument, nullptr, 'C'},
														 {"pal-def", required_argument, nullptr, 'P'},
														 {"chr-data", required_argument, nullptr, 'c'},
														 {"pal-data", required_argument, nullptr, 'p'},
														 {"png-data", required_argument, nullptr, 'N'},
														 {"output", required_argument, nullptr, 'o'},
														 {"trns", no_argument, nullptr, 't'},
														 {"trns-index", required_argument, nullptr, 'i'},
														 {"columns", required_argument, nullptr, 'd'},
														 {"subpalette", required_argument, nullptr, 's'},
														 {"to-chr", no_argument, nullptr, 'M'},
														 {"help", no_argument, nullptr, 'h'},
														 {nullptr, 0, nullptr, 0}};

	while(true) {
		const auto thisOpt = getopt_long(argc, argv, shortOpts, longOpts, nullptr);

		if(thisOpt == -1)
			break;

		ifstream gfxdef_file;
		switch(thisOpt) {
			// gfx-def
			case 'G':
				cfg.gfxdef_name = optarg;
				break;

			// chr-def
			case 'C':
				cfg.chrdef_name = optarg;
				break;

			// pal-def
			case 'P':
				cfg.paldef_name = optarg;
				break;

			// chr-data
			case 'c':
				cfg.chrdata_name = optarg;
				break;

			// pal-data
			case 'p':
				cfg.paldata_name = optarg;
				break;

			// output
			case 'o':
				cfg.outfile = optarg;
				break;

			// trns
			case 't':
				cfg.rendertraits.use_trns = true;
				break;

			// trns entry
			case 'i':
				try {
					cfg.rendertraits.trns_entry = stoi(optarg);
				} catch(const invalid_argument &e) {
					throw invalid_argument("Invalid transparency index value");
				}
				break;

			// columns
			case 'd':
				try {
					cfg.rendertraits.cols = stoi(optarg);
				} catch(const invalid_argument &e) {
					throw invalid_argument("Invalid columns value");
				}
				break;

			// subpalette
			case 's':
				try {
					cfg.subpalette = stoi(optarg);
				} catch(const invalid_argument &e) {
					throw invalid_argument("Invalid subpalette index");
				}
				break;
			case 'N':
				cfg.pngdata_name = optarg;
				break;
			case 'M':
				cfg.to_chr = true;
				break;

			// help
			case 'h':
				print_help();
				exit(0);
				break;

			case ':':
				cerr << "Missing arg for option: " << (char)optopt << endl;
				exit(1);
				break;
			case '?':
				cerr << "Unknown option: " << (char)optopt << endl;
			default:
				print_help();
				exit(1);
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
