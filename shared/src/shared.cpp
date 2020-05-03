#include "shared.hpp"
#include <iostream>
#include <vector>

class gfxprofile : public chrgfx::gfx_def
{
public:
	gfxprofile(std::string const &id, std::string const &chrdef_id,
						 std::string const &coldef_id, std::string const &paldef_id)
			: chrgfx::gfx_def(std::move(id)), chrdef_id(std::move(chrdef_id)),
				coldef_id(std::move(coldef_id)), paldef_id(std::move(paldef_id)){};

	std::string get_chrdef_id() { return chrdef_id; }
	std::string get_coldef_id() { return coldef_id; }
	std::string get_paldef_id() { return paldef_id; }

private:
	std::string chrdef_id;
	std::string coldef_id;
	std::string paldef_id;
};

std::string default_short_opts{":G:C:P:o:s:h"};

std::vector<option> default_long_opts{
		{"gfx-def", required_argument, nullptr, 'G'},
		{"chr-def", required_argument, nullptr, 'C'},
		{"pal-def", required_argument, nullptr, 'P'},

		//{"png-data", required_argument, nullptr, 'N'},
		{"output", required_argument, nullptr, 'o'},
		{"subpalette", required_argument, nullptr, 's'},
		{"help", no_argument, nullptr, 'h'}};

bool process_default_args(runtime_config &cfg, int argc, char **argv)
{
	// add the terminating element
	default_long_opts.push_back({nullptr, 0, nullptr, 0});

	while(true) {
		const auto this_opt = getopt_long(argc, argv, default_short_opts.data(),
																			default_long_opts.data(), nullptr);
		if(this_opt == -1)
			break;

		switch(this_opt) {
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

			// output
			case 'o':
				cfg.outfile = optarg;
				break;

			// subpalette
			case 's':
				try {
					cfg.subpalette = std::stoi(optarg);
				} catch(const std::invalid_argument &e) {
					throw std::invalid_argument("Invalid subpalette index");
				}
				break;
			// case 'N':
			//	cfg.pngdata_name = optarg;
			//	break;

			// help
			case 'h':
				// print_help();
				return false;
			case ':':
				std::cerr << "Missing arg for option: " << (char)optopt << std::endl;
				return false;
				break;
			case '?':
				std::cerr << "Unknown option: " << (char)optopt << std::endl;
				return false;
		}
	}
	// reset the getopt index for the next scan
	optind = 0;
	return true;
}
