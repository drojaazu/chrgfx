#include "main.hpp"

#ifdef DEBUG
#include <chrono>
#endif

using namespace std;
using namespace chrgfx;

// application globals
const string version = string("1.1");
int return_status;

// built-in graphics formats
map<string, chr_def> chrdef_list = {
		{string("mono"), chrdefs::chr_8x8x1},
		{string("sega_md"), chrdefs::chr_8x8x4_packed},
		{string("nintendo_sfc"), chrdefs::nintendo_sfc}};

map<string, pal_def> paldef_list = {
		{string("sega_md"), paldefs::sega_md_pal},
		{string("nintendo_sfc"), paldefs::nintendo_sfc_pal},
		{string("tlp"), paldefs::tilelayerpro}};

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

	bool is_internal = true;

	try {
		process_args(argc, argv);

		// SANITY CHECKING HERE
		/*
			chrdata is not required; if not present, use cin

			chrdef not required; if not present, use internal 1bpp mode

			paldata is not required; if not present,use system generated colors

			paldef required if paldata present; if present with no paldata, ignore
		*/

		/*
			if gfxdef is set, do not check chrdef or paldef
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
		// gfxdef has priority; if it is set, we ignore chr/pal def options
		if(!cfg.gfxdef_name.empty()) {
			// use chrdef/paldef from gfxdef
			ifstream gfxdef_file(cfg.gfxdef_name);
			if(gfxdef_file.good()) {
				chrdef = get_chrdef(gfxdef_file);
				gfxdef_file.clear();
				if(paldata != nullptr) {
					paldef = get_paldef(gfxdef_file);
				}
				is_internal = false;
			} else {
				throw invalid_argument("Unable to open gfxdef file");
			}
		} else {
			// use individual chrdef/paldef settings or internal settings
			if(cfg.chrdef_name.empty()) {
				chrdef = &chrdefs::chr_8x8x1;
			} else {
				ifstream chrdef_file(cfg.chrdef_name);
				if(chrdef_file.good()) {
					chrdef = get_chrdef(chrdef_file);
					is_internal = false;
				} else {
					if(chrdef_list.find(cfg.chrdef_name) == chrdef_list.end()) {
						throw invalid_argument(
								"Invalid chrdef file or internal tile format specified");
					}
					chrdef = &chrdef_list.at(cfg.chrdef_name);
					is_internal = true;
				}
			}

			// if paldata is null, we don't even bother with the paldef
			if(paldata != nullptr) {
				ifstream paldef_file(cfg.paldef_name);
				if(paldef_file.good()) {
					paldef = get_paldef(paldef_file);
					is_internal = false;
				} else {
					if(paldef_list.find(cfg.paldef_name) == paldef_list.end()) {
						throw invalid_argument(
								"Invalid paldef file or internal palette format specified");
					}
					paldef = &paldef_list.at(cfg.paldef_name);
					is_internal = true;
				}
			}
		}

		// use system palette if no palette data supplied
		if(paldata == nullptr)
			workpal = chrgfx::make_pal(false);
		else {
			// with the rules above, where there's paldata, there's a a paldef

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

	if(!is_internal) {
		delete chrdef;
		if(paldef != nullptr) {
			delete paldef->get_colordef();
			delete paldef->get_syspal();
		}
		delete paldef;
	}

	delete workpal;
	return return_status;
}

void process_args(int argc, char **argv)
{
	const char *const shortOpts = ":f:g:t:p:o:rc:s:h";
	const option longOpts[] = {{"gfx-format", required_argument, nullptr, 'G'},
														 {"chr-format", required_argument, nullptr, 'C'},
														 {"pal-format", required_argument, nullptr, 'P'},
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
			// TODO: format files
			/*
				- gfx/chr/pal format can specify either a file or an internal definition
				- file has priority; if it doesn't exist, check internal; if neither,
				error
				- gfxdef loads a chr AND a pal def; if both not found, error
				- chrdef loads a chr only, and paldef loads pal only
				- gfxdef AND chr/pal def can be specified; if so, individual chr/pal
				defs take precedence
			*/
			// gfx-format
			case 'G':
				cfg.gfxdef_name = optarg;
				/*
				gfxdef_file = ifstream(optarg);
				if(gfxdef_file.good()) {
					chrdef = get_chrdef(&gfxdef_file);
					paldef = get_paldef(&gfxdef_file);
					is_internal = false;
				} else {
					throw invalid_argument("Invalid gfxdef file specified");
				}
				*/
				break;

			// chr-format
			case 'C':
				cfg.chrdef_name = optarg;
				/*
				gfxdef_file = ifstream(optarg);
				if(gfxdef_file.good()) {
					get_chrdef(&gfxdef_file);
					is_internal = false;
				} else {
					if(chrdef_list.find(chrdef_name) == chrdef_list.end()) {
						throw invalid_argument("Invalid tile format specified");
					}
					chrdef = &chrdef_list.at(optarg);
					is_internal = true;
				}
				*/
				break;

			// pal-format
			case 'P':
				cfg.paldef_name = optarg;
				/*
				palx_name = optarg;
				gfxdef_file = ifstream(optarg);
				if(gfxdef_file.good()) {
					get_paldef(&gfxdef_file);
					is_internal = false;
				} else {
					if(paldef_list.find(palx_name) == paldef_list.end()) {
						throw invalid_argument("Invalid palette format specified");
					}
					paldef = &paldef_list.at(optarg);
					is_internal = true;
				}
				*/
				break;

			// chr-data
			case 'c':
				cfg.chrdata_name = optarg;
				// cfg.chrdata = ifstream(optarg);
				break;

			// pal-data
			case 'p':
				cfg.paldata_name = optarg;
				// cfg.paldata = ifstream(optarg);
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
	cerr << "  --gfx-format, -G   Specify graphics data format" << endl;
	cerr << "  --chr-format, -C   Specify tile data format" << endl;
	cerr << "  --chr-data, -c     Filename to input tile data" << endl;
	cerr << "  --pal-format, -P   Specify palette data format" << endl;
	cerr << "  --pal-data, -p     Filename to input palette data" << endl;
	cerr << "  --output, -o       Specify output PNG image filename" << endl;
	cerr << "  --trns, -t         Use image transparency" << endl;
	cerr << "  --trns-index, -i   Specify palette entry to use as transparency "
					"(default is 0)"
			 << endl;
	cerr << "  --columns, -c      Specify number of columns per row of tiles in "
					"output image"
			 << endl;
	cerr << "  --subpalette, -s   Specify palette entry at which to begin "
					"(default is 0)"
			 << endl;
	cerr << "  --help, -h         Display this text" << endl;
}
