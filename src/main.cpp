
#include "main.hpp"

using namespace std;
using namespace gfx;

const map<string, chr_xform*> chrx_list = {
		{string("sega_md"), new sega_md_cx()},
		{string("nintendo_sfc"), new nintendo_sfc_cx()}};

const map<string, pal_xform*> palx_list = {
		{string("sega_md"), new sega_md_px()},
		{string("nintendo_sfc"), new nintendo_sfc_px()},
		{string("tilelayerpro"), new tilelayerpro_px()}};

string outfile;

render_traits rtraits;

istream* chr_data = nullptr;
istream* pal_data = nullptr;

chr_xform* chrx = nullptr;
pal_xform* palx = nullptr;

const palette* work_pal;
bank work_bank;


int main(int argc, char** argv)
{
	try
	{
		process_args(argc, argv);

		if(pal_data == nullptr)
			work_pal = gfx::make_pal();
		else
		{
			pal_data->seekg(0, pal_data->end);
			int length = pal_data->tellg();
			pal_data->seekg(0, pal_data->beg);
			auto palbuffer = new char[length];
			pal_data->read(palbuffer, length);
			work_pal = palx->get_pal((uint8_t*)palbuffer);
			delete palbuffer;
			delete pal_data;
		}

		if(chr_data == nullptr)
			chr_data = &cin;
		else
			chr_data->seekg(0);
		// read input from either stream or file for tile data
		// read file input for palette data

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

		// 5/31
		// at last check, things were mostly working with command line stuff
		// do some more testing, flesh it out, clean up some of this messy code

		// add some proper error checking

		auto chunkSize = chrx->get_traits()->data_size;
		auto thisChunk = new char[chunkSize];

		while(!chr_data->eof())
		{
			chr_data->read(thisChunk, chunkSize);
			// what does read() do if we run out of bytes?
			work_bank.push_back(chrx->get_chr((uint8_t*)thisChunk));
		}

		png::image<png::index_pixel>* outimg =
				render(&work_bank, work_pal, &rtraits);

		if(outfile.empty())
			outimg->write_stream(cout);
		else
			outimg->write(outfile);
	}

	catch(const exception& e)
	{
		cerr << "Fatal error: " << e.what() << endl;
		return -1;
	}

	return 0;
}

void process_args(int argc, char** argv)
{
	const char* const shortOpts = "f:g:t:p:o:r:c:s";
	const option longOpts[] = {
			{"tile-format", required_argument, nullptr, 'f'},
			{"palette-format", required_argument, nullptr, 'g'},
			{"tile-data", required_argument, nullptr, 't'},
			{"palette-data", required_argument, nullptr, 'p'},
			{"output", required_argument, nullptr, 'o'},
			{"trns", no_argument, nullptr, 'r'},
			{"columns", required_argument, nullptr, 'c'},
			{"palette-shift", required_argument, nullptr, 's'},
			{nullptr, 0, nullptr, 0}};

	while(true)
	{
		const auto thisOpt = getopt_long(argc, argv, shortOpts, longOpts, nullptr);

		if(thisOpt == -1) break;

		switch(thisOpt)
		{
			// tile-format
			case 'f':
				chrx = chrx_list.at(optarg);
				break;

			// palette-format
			case 'g':
				palx = palx_list.at(optarg);
				break;

			// tile-data
			case 't':
				chr_data = new ifstream(optarg);
				break;

			// palette-data
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

			// columns
			case 'c':
				rtraits.cols = stoi(optarg);
				break;

			// palette-shift
			case 's':
				rtraits.palette_offset = stoi(optarg);
				break;

			// help
			case 'h':
			case '?':
			default:
				print_help();
				break;
		}
	}
}

void print_help()
{
	cout << "help screen here" << endl;
	exit(1);
}
