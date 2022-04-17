#include "shared.hpp"

using namespace std;

string const GFXDEF_PATH { CONFIG_PATH + "/gfxdefs" };
string const CONFIG_PATH { "/etc/chrgfx" };

string short_opts { ":G:T:C:L:P:" };

vector<option> long_opts { { "gfx-def", required_argument, nullptr, 'G' },
													 { "profile", required_argument, nullptr, 'P' },
													 { "chr-def", required_argument, nullptr, 'T' },
													 { "col-def", required_argument, nullptr, 'C' },
													 { "pal-def", required_argument, nullptr, 'L' } };

vector<option_details> opt_details {
	{ false, L"Path to graphics encoding definitions", L"path" },
	{ false, L"Graphics profile to use", nullptr },
	{ false,
		L"Tile encoding to use; overrides tile encoding in graphics profile "
		L"(if specified)",
		nullptr },
	{ false,
		L"Color encoding to use; overrides color encoding in graphics "
		L"profile (if specified)",
		nullptr },
	{ false,
		L"Palette encoding to use; overrides palette encoding in "
		L"graphics profile (if specified)",
		nullptr }
};

bool process_default_args(runtime_config & cfg, int argc, char ** argv)
{
	// add the terminating element
	long_opts.push_back({ nullptr, 0, nullptr, 0 });

	while(true)
	{
		const auto this_opt =
				getopt_long(argc, argv, short_opts.data(), long_opts.data(), nullptr);
		if(this_opt == -1)
			break;

		switch(this_opt)
		{
			// gfx-def
			case 'G':
				cfg.gfxdefs_path = optarg;
				break;

			case 'T':
				cfg.chrdef_id = optarg;
				break;

			case 'C':
				cfg.coldef_id = optarg;
				break;

			case 'L':
				cfg.paldef_id = optarg;
				break;

			case 'P':
				cfg.profile = optarg;
				break;

			case ':':
				std::cerr << "Missing arg for option: " << std::to_string(optopt)
									<< std::endl;
				return false;
				break;
			case '?':
				std::cerr << "Unknown argument" << std::endl;
				return false;
		}
	}

	// reset the getopt index for the next scan
	optind = 0;
	return true;
}
