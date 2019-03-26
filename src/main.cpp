#include "main.hpp"
#ifdef DEBUG
#include <chrono>
#endif

using namespace std;
using namespace chrgfx;

const string version = string("0.1");

const map<string, chr_xform*> chrx_list = {
		{string("1bpp"), new bpp1_cx()},
		{string("sega_md"), new sega_md_cx()},
		{string("nintendo_sfc"), new nintendo_sfc_cx()},
		{string("nintendo_sfc_3bpp"), new nintendo_sfc_3bpp_cx()},
		{string("nintendo_sfc_8bpp"), new nintendo_sfc_8bpp_cx()},
		{string("nintendo_fc"), new nintendo_fc_cx()},
		{string("nintendo_gb"), new nintendo_gb_cx()},
		{string("nintendo_vb"), new nintendo_vboy_cx()},
		{string("capcom_cps"), new capcom_cps_cx()},
		{string("sega_8bit"), new sega_8bit_cx()},
		{string("snk_neogeo"), new snk_neogeo_cx()},
		{string("snk_neogeocd"), new snk_neogeocd_cx()},
		{string("snk_ngp"), new snk_ngp_cx()}};

const map<string, pal_xform*> palx_list = {
		{string("sega_md"), new sega_md_px()},
		{string("tilelayerpro"), new tilelayerpro_px()},
		{string("sega_sms"), new sega_mastersys_px()},
		{string("sega_gg"), new sega_gamegear_px()},
		{string("nintendo_fc"), new nintendo_fc_px()},
		{string("nintendo_sfc"), new nintendo_sfc_px()},
		{string("nintendo_gb"), new nintendo_gb_px()},
		{string("nintendo_gb_pocket"), new nintendo_gbpocket_px()},
		{string("nintendo_gb_color"), new nintendo_gbcolor_px()},
		{string("nintendo_vb"), new nintendo_vboy_px()},
		{string("snk_neogeo"), new snk_neogeo_px()},
		{string("snk_ngp"), new snk_ngp_px()},
		{string("snk_ngpc"), new snk_ngpc_px()}};

string outfile, chrx_name, palx_name;

render_traits rtraits;

istream* chr_data = nullptr;
istream* pal_data = nullptr;

chr_xform* chrx = nullptr;
pal_xform* palx = nullptr;

const palette* work_pal;
s16 subpalette{-1};

int main(int argc, char** argv)
{
	try
	{
		process_args(argc, argv);

		// set defaults & check sanity
		// default to 1bpp if no chrx specified
		if(chrx == nullptr) chrx = chrx_list.at("1bpp");

		// if no pal format was passed, see if there is a palx with same name as the
		// chrx
		if(palx == nullptr && palx_list.find(chrx_name) != palx_list.end())
		{
			palx_name = chrx_name;
			palx = palx_list.at(chrx_name);
		}

		if(chr_data == nullptr)
			chr_data = &cin;
		else
		{
			if(!chr_data->good())
			{
				cerr << "CHR data file could not be opened" << endl;
				return 2;
			}
			chr_data->seekg(0);
		}

		// use system palette if no palette data supplied
		if(pal_data == nullptr)
			work_pal = chrgfx::make_pal();
		else
		{
			if(!pal_data->good())
			{
				cerr << "PAL data file could not be opened" << endl;
				return 2;
			}
			pal_data->seekg(0, pal_data->end);
			int length = pal_data->tellg();
			pal_data->seekg(0, pal_data->beg);
			auto palbuffer = new char[length];
			pal_data->read(palbuffer, length);
			work_pal = palx->get_pal((uint8_t*)palbuffer, subpalette);
			delete[] palbuffer;
			delete pal_data;
		}

		bank work_bank = bank(*chrx->get_traits());
		/*
		stream read psuedocode
		1. get data size of tile from converter traits = x
		2. allocate array with x bytes
		3. read x bytes into array
		3a. not enough bytes, fill with 0s
		4. call get tile on the array to get a processed tile
		5. add tile to vector for rendering
		6. repeat until end of stream

		*/
#ifdef DEBUG
		std::chrono::high_resolution_clock::time_point t1 =
				std::chrono::high_resolution_clock::now();
#endif
		auto chunksize = chrx->get_traits()->data_size;
		auto chunkbuffer = new char[chunksize];

		while(!chr_data->eof())
		{
			chr_data->read(chunkbuffer, chunksize);
			// what does read() do if we run out of bytes?
			work_bank.data()->push_back(chrx->get_chr((u8*)chunkbuffer));
		}

		if(chr_data != &cin) delete chr_data;
		delete[] chunkbuffer;

#ifdef DEBUG
		std::chrono::high_resolution_clock::time_point t2 =
				std::chrono::high_resolution_clock::now();
		auto duration =
				std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count();

		cerr << "Tile conversion: " << duration << endl;

		t1 = std::chrono::high_resolution_clock::now();
#endif

		png::image<png::index_pixel>* outimg =
				render(&work_bank, work_pal, &rtraits);

#ifdef DEBUG
		t2 = std::chrono::high_resolution_clock::now();
		duration =
				std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count();
		cerr << "Rendering: " << duration << endl;
#endif

		if(outfile.empty())
			outimg->write_stream(cout);
		else
			outimg->write(outfile);

		delete outimg;
		free_vectors();
	}

	catch(const exception& e)
	{
		free_vectors();
		cerr << "Fatal error: " << e.what() << endl;
		return -1;
	}

	return 0;
}

void free_vectors()
{
	for(auto const& this_chrx : chrx_list) delete this_chrx.second;

	for(auto const& this_palx : palx_list) delete this_palx.second;
}

void process_args(int argc, char** argv)
{
	const char* const shortOpts = ":f:g:t:p:o:rc:s:h";
	const option longOpts[] = {{"chr-format", required_argument, nullptr, 'f'},
														 {"pal-format", required_argument, nullptr, 'g'},
														 {"chr-data", required_argument, nullptr, 't'},
														 {"pal-data", required_argument, nullptr, 'p'},
														 {"output", required_argument, nullptr, 'o'},
														 {"trns", no_argument, nullptr, 'r'},
														 {"trns-index", required_argument, nullptr, 'i'},
														 {"columns", required_argument, nullptr, 'c'},
														 {"subpalette", required_argument, nullptr, 's'},
														 {"help", no_argument, nullptr, 'h'},
														 {nullptr, 0, nullptr, 0}};

	while(true)
	{
		const auto thisOpt = getopt_long(argc, argv, shortOpts, longOpts, nullptr);

		if(thisOpt == -1) break;

		switch(thisOpt)
		{
			// chr-format
			case 'f':
				chrx_name = optarg;
				if(chrx_list.find(chrx_name) == chrx_list.end())
					throw invalid_argument("Invalid CHR format specified");
				chrx = chrx_list.at(optarg);
				break;

			// pal-format
			case 'g':
				palx_name = optarg;
				if(palx_list.find(palx_name) == palx_list.end())
					throw invalid_argument("Invalid palette format specified");
				palx = palx_list.at(optarg);
				break;

			// chr-data
			case 't':
				chr_data = new ifstream(optarg);
				break;

			// pal-data
			case 'p':
				pal_data = new ifstream(optarg);
				break;

			// output
			case 'o':
				outfile = optarg;
				break;

			// trns
			case 'r':
				rtraits.use_trns = true;
				break;
			// trns entry
			case 'i':
				try
				{
					rtraits.trns_entry = stoi(optarg);
				}
				catch(const invalid_argument& e)
				{
					throw invalid_argument("Invalid transparency index value");
				}
				break;

			// columns
			case 'c':
				try
				{
					rtraits.cols = stoi(optarg);
				}
				catch(const invalid_argument& e)
				{
					throw invalid_argument("Invalid columns value");
				}
				break;

			// subpalette
			case 's':
				subpalette = stoi(optarg);
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
	cerr << "  --chr-format, -f   Specify tile data format" << endl;
	cerr << "  --chr-data, -t     Filename to input tile data" << endl;
	cerr << "  --pal-format, -g   Specify palette data format" << endl;
	cerr << "  --pal-data, -p     Filename to input palette data" << endl;
	cerr << "  --output, -o       Specify output PNG image filename" << endl;
	cerr << "  --trns, -r         Use image transparency" << endl;
	cerr << "  --trns-entry, -i   Specify palette entry to use as transparency "
					"(default is 0)"
			 << endl;
	cerr << "  --columns, -c      Specify number of columns per row of tiles in "
					"output image"
			 << endl;
	cerr << "  --pal-offset, -s   Specify palette entry at which to begin "
					"(default is 0)"
			 << endl;
	cerr << "  --help, -h         Display this text" << endl;
}
