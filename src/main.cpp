#include "main.hpp"
#ifdef DEBUG
#include <chrono>
#endif

// TODO - rearrange all the helper functions/PODs!

using namespace std;
using namespace chrgfx;

const string version = string("1.1");

const map<string, const chr_def> chrdef_list = {
		{string("mono"), chrdefs::chr_8x8x1},
		{string("sega_md"), chrdefs::chr_8x8x4_packed},
		{string("sega_8bit"), chrdefs::sega_8bit},
		{string("nintendo_sfc"), chrdefs::nintendo_sfc},
		{string("nintendo_fc"), chrdefs::nintendo_fc},
		{string("nintendo_gb"), chrdefs::nintendo_2bpp},
		{string("capcom_cps1"), chrdefs::capcom_cps1},
		{string("snk_neogeo"), chrdefs::snk_neogeo},
		{string("snk_neogeocd"), chrdefs::snk_neogeocd},
		{string("snk_neogeo_pocket"), chrdefs::snk_neogeo_pocket},
		{string("seta"), chrdefs::seta_chr},
		{string("seta_sprites"), chrdefs::seta_sprites}};

const map<string, pal_def> paldef_list = {
		{string("sega_md"), paldefs::sega_md_pal},
		{string("nintendo_gb_classic"), paldefs::nintendo_gb_classic_pal},
		{string("nintendo_fc"), paldefs::nintendo_fc_pal},
		{string("nintendo_sfc"), paldefs::nintendo_sfc_pal},
		{string("snk_neogeo"), paldefs::snk_neogeo_pal},
		{string("snk_neogeo_noshadow"), paldefs::snk_neogeo_noshadow_pal},
		{string("snk_neogeo_pocket"), paldefs::snk_neogeo_pocket_pal},
		{string("tlp"), paldefs::tilelayerpro},
		{string("seta"), paldefs::seta_pal}};

string outfile, chrdef_name, palx_name;

render_traits rtraits;

istream* chr_data = nullptr;
istream* pal_data = nullptr;

pal_def paldef(paldefs::sega_md_pal);
chr_def chrdef(chrdefs::chr_8x8x1);

palette work_pal;
s16 subpalette{-1};

int main(int argc, char** argv)
{
	try
	{
		process_args(argc, argv);

		// set defaults & check sanity

		// if no pal format was passed, see if there is a palx with same name as the
		// chrx
		// if(palx == nullptr && paldef_list.find(chrdef_name) != palx_list.end())
		//{
		//	palx_name = chrdef_name;
		//	palx = palx_list.at(chrdef_name);
		//}

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
			work_pal = *(chrgfx::make_pal(false));
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
			work_pal = paldef.decoder(&paldef, (u8*)palbuffer, subpalette);
			delete[] palbuffer;
			delete pal_data;
		}

		bank work_bank = bank(chrdef);
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
		// auto chunksize = chrx->get_traits()->data_size;

		const u16 chunksize = (chrdef.charincrement / 8);
		auto chunkbuffer = new char[chunksize];

		while(!chr_data->eof())
		{
			chr_data->read(chunkbuffer, chunksize);
			// what does read() do if we run out of bytes?
			work_bank.data()->push_back(get_chr(&chrdef, (u8*)chunkbuffer));
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
				render(&work_bank, &work_pal, &rtraits);

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
	// for(auto const& this_palx : palx_list) delete this_palx.second;
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
				chrdef_name = optarg;
				if(chrdef_list.find(chrdef_name) == chrdef_list.end())
					throw invalid_argument("Invalid CHR format specified");
				chrdef = chrdef_list.at(optarg);
				break;

			// pal-format
			case 'g':
				palx_name = optarg;
				if(paldef_list.find(palx_name) == paldef_list.end())
					throw invalid_argument("Invalid palette format specified");
				paldef = paldef_list.at(optarg);
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
				try
				{
					subpalette = stoi(optarg);
				}
				catch(const invalid_argument& e)
				{
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
	cerr << "  --chr-format, -f   Specify tile data format" << endl;
	cerr << "  --chr-data, -t     Filename to input tile data" << endl;
	cerr << "  --pal-format, -g   Specify palette data format" << endl;
	cerr << "  --pal-data, -p     Filename to input palette data" << endl;
	cerr << "  --output, -o       Specify output PNG image filename" << endl;
	cerr << "  --trns, -r         Use image transparency" << endl;
	cerr << "  --trns-index, -i   Specify palette entry to use as transparency "
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
