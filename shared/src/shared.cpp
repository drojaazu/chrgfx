#include "shared.hpp"

using std::string;

string default_short_opts{":G:P:s:h"};

std::vector<option> default_long_opts{
		{"gfx-def", required_argument, nullptr, 'G'},
		{"chr-def", required_argument, nullptr, 'T'},
		{"col-def", required_argument, nullptr, 'C'},
		{"pal-def", required_argument, nullptr, 'L'},
		{"profile", required_argument, nullptr, 'P'},
		{"subpalette", required_argument, nullptr, 's'}};

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
				cfg.gfxdef = optarg;
				break;

			case 'T':
				cfg.chrdef = optarg;
				break;

			case 'C':
				cfg.coldef = optarg;
				break;

			case 'L':
				cfg.paldef = optarg;
				break;

			case 'P':
				cfg.profile = optarg;
				break;

			// subpalette
			case 's':
				try {
					cfg.subpalette = std::stoi(optarg);
				} catch(const std::invalid_argument &e) {
					throw std::invalid_argument("Invalid subpalette index");
				}
				break;

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
