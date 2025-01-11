#include "shared.hpp"
#include <iostream>

using namespace std;

// command line argument processing
string short_opts {":G:P:T:C:L:ah"};

int longopt_idx {0};
vector<option> long_opts {// predefined gfx defs
	{"gfx-def", required_argument, nullptr, 'G'},
	{"profile", required_argument, nullptr, 'P'},
	{"chr-def", required_argument, nullptr, 'T'},
	{"col-def", required_argument, nullptr, 'C'},
	{"pal-def", required_argument, nullptr, 'L'},
	{"list-gfxdefs", no_argument, nullptr, 'a'},
	// cli defined gfx defs - chr
	// (we redirect values for long options into opt_idx so that the return value can never creep up into the print ascii
	// range for short opts)
	{"chr-width", required_argument, &longopt_idx, 0},
	{"chr-height", required_argument, &longopt_idx, 1},
	{"chr-bpp", required_argument, &longopt_idx, 2},
	{"chr-plane-offsets", required_argument, &longopt_idx, 3},
	{"chr-pixel-offsets", required_argument, &longopt_idx, 4},
	{"chr-row-offsets", required_argument, &longopt_idx, 5},
	{"col-bitdepth", required_argument, &longopt_idx, 6},
	{"col-layout", required_argument, &longopt_idx, 7},
	{"col-big-endian", required_argument, &longopt_idx, 8},

	{"help", no_argument, nullptr, 'h'}};

vector<option_details> opt_details {
	// predefined gfx defs
	{false, "Path to graphics encoding definitions", "path"},
	{false, "Graphics profile to use", nullptr},
	{false, "Tile encoding to use; overrides tile encoding in graphics profile (if specified)", nullptr},
	{false, "Color encoding to use; overrides color encoding in graphics profile (if specified)", nullptr},
	{false, "Palette encoding to use; overrides palette encoding in graphics profile (if specified)", nullptr},
	{false, "List all available encodings in gfxdefs file", nullptr},
	// cli defined gfx defs - chr
	{false, "Tile width", nullptr},
	{false, "Tile height", nullptr},
	{false, "Tile color bits per pixel", nullptr},
	{false, "Tile plane offsets", nullptr},
	{false, "Tile pixel offsets", nullptr},
	{false, "Tile row offsets", nullptr},

	{false, "Display program usage", nullptr},
};

bool shared_args(char this_opt, runtime_config & cfg)
{
	bool found = true;
	switch (this_opt)
	{
		// non-short options
		case 0:
			switch (longopt_idx)
			{
				case 0:
					cfg.chrdef_width = optarg;
					break;
				case 1:
					cfg.chrdef_height = optarg;
					break;
				case 2:
					cfg.chrdef_bpp = optarg;
					break;
				case 3:
					cfg.chrdef_plane_offsets = optarg;
					break;
				case 4:
					cfg.chrdef_pixel_offsets = optarg;
					break;
				case 5:
					cfg.chrdef_row_offsets = optarg;
					break;
				case 6:
					cfg.rgbcoldef_bitdepth = optarg;
					break;
				case 7:
					cfg.rgbcoldef_rgblayout = optarg;
					break;
				case 8:
					cfg.rgbcoldef_big_endian = optarg;
					break;
				default:
					break;
			}
			break;

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
			cfg.profile_id = optarg;
			break;

		case 'h':
			cout << show_usage(long_opts.data(), opt_details.data()) << '\n';
			exit(0);

		case ':':
			cerr << "Missing arg for option: " << to_string(optopt) << '\n';
			exit(-1);

		case '?':
			cerr << "Unknown argument" << '\n';
			exit(-2);

		default:
			found = false;
	}

	return found;
}

bool runtime_config::chrdef_cli_defined()
{
	return ! (chrdef_bpp.empty() && chrdef_width.empty() && chrdef_height.empty() && chrdef_pixel_offsets.empty() &&
						chrdef_plane_offsets.empty() && chrdef_row_offsets.empty());
}

bool runtime_config::coldef_cli_defined()
{
	return ! (rgbcoldef_big_endian.empty() && rgbcoldef_rgblayout.empty() && rgbcoldef_bitdepth.empty());
}

bool runtime_config::paldef_cli_defined()
{
	return ! (paldef_datasize.empty() && paldef_entry_datasize.empty() && paldef_length.empty());
}
