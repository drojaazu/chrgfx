#include "shared.hpp"
#include "gfxdef_builder.hpp"
#include "strutil.hpp"
#include "xdgdirs.hpp"

using namespace std;

string const CONFIG_PATH {"/etc/chrgfx"};
string const GFXDEF_PATH {CONFIG_PATH + "/gfxdefs"};

def_helper::def_helper(runtime_config & cfg) :
		m_defs(load_gfxdefs(cfg.gfxdefs_path.empty() ? data_filepaths("chrgfx/gfxdefs").front() : cfg.gfxdefs_path))
{
	if (cfg.list_gfxdefs)
	{
		list_gfxdefs(cout);
		exit(0);
	}

	string chrdef_id, coldef_id, paldef_id;

	// configure from gfxprofile if specified
	if (! cfg.profile.empty())
	{
		auto i_profile {m_defs.profiles.find(cfg.profile)};
		if (i_profile == m_defs.profiles.end())
		{
			ostringstream oss;
			oss << "Could not find specified gfx profile: " << cfg.profile;
			throw invalid_argument(oss.str());
		}

		chrdef_id = i_profile->second.chrdef_id();
		coldef_id = i_profile->second.coldef_id();
		paldef_id = i_profile->second.paldef_id();
	}

	// specific gfxdefs override profile settings
	if (! cfg.chrdef_id.empty())
		chrdef_id = cfg.chrdef_id;
	if (! cfg.coldef_id.empty())
		coldef_id = cfg.coldef_id;
	if (! cfg.paldef_id.empty())
		paldef_id = cfg.paldef_id;

	// load the requests defs
	if (! chrdef_id.empty())
	{
		auto iter_find_chrdef {m_defs.chrdefs.find(chrdef_id)};
		if (iter_find_chrdef == m_defs.chrdefs.end())
		{
			ostringstream oss;
			oss << "Could not find specified chrdef: " << chrdef_id;
			throw invalid_argument(oss.str());
		}
		chrdef = iter_find_chrdef->second;
	}

	if (! coldef_id.empty())
	{
		auto iter_find_rgbcoldef {m_defs.rgbcoldefs.find(coldef_id)};
		if (iter_find_rgbcoldef != m_defs.rgbcoldefs.end())
		{
			coldef = iter_find_rgbcoldef->second;
		}
		else
		{
			auto iter_find_refcoldef {m_defs.refcoldefs.find(coldef_id)};
			if (iter_find_refcoldef != m_defs.refcoldefs.end())
			{
				coldef = iter_find_refcoldef->second;
			}
			else
			{
				ostringstream oss;
				oss << "Could not find specified coldef: " << coldef_id;
				throw invalid_argument(oss.str());
			}
		}
	}

	if (! paldef_id.empty())
	{
		auto i_find_paldef {m_defs.paldefs.find(paldef_id)};
		if (i_find_paldef == m_defs.paldefs.end())
			throw invalid_argument("Could not find specified palette encoding (paldef)");
		paldef = i_find_paldef->second;
	}

	// build def(s) from scratch or override parts of the loaded def(s)
	if ((! cfg.chrdef_bpp.empty()) || (! cfg.chrdef_width.empty()) || (! cfg.chrdef_height.empty()) ||
			(! cfg.chrdef_pixel_offsets.empty()) || (! cfg.chrdef_plane_offsets.empty()) ||
			(! cfg.chrdef_row_offsets.empty()))
	{
		chrdef_builder builder(chrdef);
		builder.set_id("chrdef_cli_generated");
		if (! cfg.chrdef_width.empty())
			builder.set_width(cfg.chrdef_width);
		if (! cfg.chrdef_height.empty())
			builder.set_height(cfg.chrdef_height);
		if (! cfg.chrdef_bpp.empty())
			builder.set_bitdepth(cfg.chrdef_bpp);
		if (! cfg.chrdef_plane_offsets.empty())
			builder.set_plane_offsets(cfg.chrdef_plane_offsets);
		if (! cfg.chrdef_pixel_offsets.empty())
			builder.set_pixel_offsets(cfg.chrdef_pixel_offsets);
		if (! cfg.chrdef_row_offsets.empty())
			builder.set_row_offsets(cfg.chrdef_row_offsets);
		chrdef = builder.build();
	}
}

void def_helper::list_gfxdefs(ostream & os)
{
	for (auto const & profile : m_defs.profiles)
	{
		os << "profile " << profile.second.id();
		if (! profile.second.description().empty())
			os << " (" << profile.second.description() << ')';
		os << '\n';
	}

	for (auto const & chrdef : m_defs.chrdefs)
	{
		os << "chrdef " << chrdef.second.id();
		if (! chrdef.second.description().empty())
			os << " (" << chrdef.second.description() << ')';
		os << '\n';
	}

	for (auto const & paldef : m_defs.paldefs)
	{
		os << "paldef " << paldef.second.id();
		if (! paldef.second.description().empty())
			os << " (" << paldef.second.description() << ')';
		os << '\n';
	}

	for (auto const & rgbcoldef : m_defs.rgbcoldefs)
	{
		os << "coldef " << rgbcoldef.second.id();
		if (! rgbcoldef.second.description().empty())
			os << " (" << rgbcoldef.second.description() << ')';
		os << '\n';
	}
	for (auto const & refcoldef : m_defs.refcoldefs)
	{
		os << "coldef " << refcoldef.second.id();
		if (! refcoldef.second.description().empty())
			os << " (" << refcoldef.second.description() << ')';
		os << '\n';
	}
	os.flush();
}

// command line argument processing
string short_opts {":G:P:T:C:L:lh"};

int longopt_idx {0};
vector<option> long_opts {// predefined gfx defs
	{"gfx-def", required_argument, nullptr, 'G'},
	{"profile", required_argument, nullptr, 'P'},
	{"chr-def", required_argument, nullptr, 'T'},
	{"col-def", required_argument, nullptr, 'C'},
	{"pal-def", required_argument, nullptr, 'L'},
	{"list-gfxdefs", no_argument, nullptr, 'l'},
	// cli defined gfx defs - chr
	// (we redirect values for long options into opt_idx so that the return value can never creep up into the print ascii
	// range for short opts)
	{"chr-width", required_argument, &longopt_idx, 0},
	{"chr-height", required_argument, &longopt_idx, 1},
	{"chr-bpp", required_argument, &longopt_idx, 2},
	{"chr-plane-offsets", required_argument, &longopt_idx, 3},
	{"chr-pixel-offsets", required_argument, &longopt_idx, 4},
	{"chr-row-offsets", required_argument, &longopt_idx, 5},

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
			cfg.profile = optarg;
			break;

		case 'l':
			cfg.list_gfxdefs = true;
			break;

		case 'h':
			cout << show_usage(long_opts.data(), opt_details.data()) << endl;
			exit(0);

		case ':':
			cerr << "Missing arg for option: " << to_string(optopt) << endl;
			exit(-1);

		case '?':
			cerr << "Unknown argument" << endl;
			exit(-2);

		default:
			found = false;
	}

	return found;
}
