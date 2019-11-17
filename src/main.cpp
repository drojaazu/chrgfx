#include "main.hpp"

#ifdef DEBUG
#include <chrono>
#endif

using namespace std;
using namespace chrgfx;

// application globals
const static string version = string("1.0");
int return_status;

// option settings
runtime_config cfg;

int main(int argc, char **argv)
{
	// declare our heap variables here in case they need
	// to be deleted from exception
	istream *chrdata{nullptr};
	istream *paldata{nullptr};
	chr_def *chrdef{nullptr};
	pal_def *paldef{nullptr};

	palette *workpal{nullptr};

	try {
		process_args(argc, argv);

		// SANITY CHECKING HERE
		/*
			chrdata is not required; if not present, use cin
			paldata is not required; if not present,use system generated colors

			chrdef always required
			paldef required if paldata present; if present with no paldata, ignore
		*/

		// check data sanity
		if(cfg.chrdata_name.empty()) {
			chrdata = &cin;
		} else {
			chrdata = new ifstream(cfg.chrdata_name);
			if(chrdata->fail()) {
				throw invalid_argument("Tile data file could not be opened");
			}
		}

		if(cfg.paldata_name.empty()) {
			paldata = nullptr;
		} else {
			paldata = new ifstream(cfg.paldata_name);
			if(paldata->fail()) {
				throw invalid_argument("Palette data file could not be opened");
			}
		}

		// check def sanity
		// check for --gfx-def first, then override with
		// --chr-def and --pal-def if present
		if(!cfg.gfxdef_name.empty()) {
			ifstream gfxdef_file(cfg.gfxdef_name);
			if(gfxdef_file.fail()) {
				throw invalid_argument("Unable to open gfxdef file");
			}
			// only bother importing if chrdef/paldef (which will override gfxdef) are
			// not defined
			if(cfg.chrdef_name.empty()) {
				chrdef = get_chrdef(gfxdef_file);
			}
			// even if paldef is set, if paldata is null, don't worry about
			// the paldef import
			if(cfg.paldef_name.empty() & paldata != nullptr) {
				gfxdef_file.clear();
				paldef = get_paldef(gfxdef_file);
			}
		}

		// use individual chrdef/paldef settings
		if(!cfg.chrdef_name.empty()) {
			ifstream chrdef_file(cfg.chrdef_name);
			if(chrdef_file.fail()) {
				throw invalid_argument("Unable to open gfxdef file in chr-def option");
			}
			// delete chrdef in case it was previously created with gfxdef option
			delete chrdef;
			chrdef = get_chrdef(chrdef_file);
			if(chrdef == nullptr) {
				throw invalid_argument("Invalid chrdef file");
			}
		}

		// if paldata is null, we don't even bother with the paldef
		if(paldata != nullptr && !cfg.paldef_name.empty()) {
			ifstream paldef_file(cfg.paldef_name);
			if(paldef_file.fail()) {
				throw invalid_argument("Unable to open gfxdef file in pal-def option");
			}
			// delete paldef in case it was previously created with gfxdef option
			delete paldef;
			paldef = get_paldef(paldef_file);
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
	}

	catch(const exception &e) {
		cerr << "Fatal error: " << e.what() << endl;
		return_status = -1;
		goto cleanup;
	}

cleanup:
	delete chrdata;
	delete paldata;

	delete chrdef;
	if(paldef != nullptr) {
		delete paldef->get_coldef();
		delete paldef->get_syspal();
	}
	delete paldef;
	delete workpal;
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
														 {"output", required_argument, nullptr, 'o'},
														 {"trns", no_argument, nullptr, 't'},
														 {"trns-index", required_argument, nullptr, 'i'},
														 {"columns", required_argument, nullptr, 'd'},
														 {"subpalette", required_argument, nullptr, 's'},
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
