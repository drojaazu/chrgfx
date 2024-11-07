#include "shared.hpp"
#include "builtin_defs.hpp"
#include "coldef.hpp"
#include "gfxdef_builder.hpp"
#include "xdgdirs.hpp"

using namespace std;

string const GFXDEF_SUBDIR {"chrgfx/gfxdefs"};

string get_gfxdefs_path()
{
	auto xdg_locations = data_filepaths(GFXDEF_SUBDIR);
	if (xdg_locations.empty())
		throw runtime_error("Could not find gfxdef file in any default location");
	return xdg_locations.front();
}

gfxprofile const & def_helper::find_gfxprofile(string const & gfxprofile_id)
{
	auto iter_find_gfxprofile {m_defs.profiles.find(gfxprofile_id)};
	if (iter_find_gfxprofile != m_defs.profiles.end())
		return iter_find_gfxprofile->second;

	ostringstream oss;
	oss << "Could not find gfxprofile: " << gfxprofile_id;
	throw invalid_argument(oss.str());
}

chrdef const * def_helper::find_chrdef(string const & chrdef_id)
{
	auto iter_find_external_chrdef {m_defs.chrdefs.find(chrdef_id)};
	if (iter_find_external_chrdef != m_defs.chrdefs.end())
		return &iter_find_external_chrdef->second;

	auto iter_find_internal_chrdef {chrgfx::gfxdefs::chrdefs.find(chrdef_id)};
	if (iter_find_internal_chrdef != chrgfx::gfxdefs::chrdefs.end())
		return &iter_find_internal_chrdef->second;

	ostringstream oss;
	oss << "Could not find specified chrdef: " << chrdef_id;
	throw invalid_argument(oss.str());
}

coldef const * def_helper::find_coldef(string const & coldef_id)
{
	auto iter_find_external_rgbcoldef {m_defs.rgbcoldefs.find(coldef_id)};
	if (iter_find_external_rgbcoldef != m_defs.rgbcoldefs.end())
		return &iter_find_external_rgbcoldef->second;

	auto iter_find_external_refcoldef {m_defs.refcoldefs.find(coldef_id)};
	if (iter_find_external_refcoldef != m_defs.refcoldefs.end())
		return &iter_find_external_refcoldef->second;

	auto iter_find_internal_rgbcoldef {chrgfx::gfxdefs::rgbcoldefs.find(coldef_id)};
	if (iter_find_internal_rgbcoldef != chrgfx::gfxdefs::rgbcoldefs.end())
		return &iter_find_internal_rgbcoldef->second;

	ostringstream oss;
	oss << "Could not find specified coldef: " << coldef_id;
	throw invalid_argument(oss.str());
}

paldef const * def_helper::find_paldef(string const & paldef_id)
{

	auto iter_find_external_paldef {m_defs.paldefs.find(paldef_id)};
	if (iter_find_external_paldef != m_defs.paldefs.end())
		return &iter_find_external_paldef->second;

	auto iter_find_internal_paldef {chrgfx::gfxdefs::paldefs.find(paldef_id)};
	if (iter_find_internal_paldef != chrgfx::gfxdefs::paldefs.end())
		return &iter_find_internal_paldef->second;

	ostringstream oss;
	oss << "Could not find specified paldef: " << paldef_id;
	throw invalid_argument(oss.str());
}

bool def_helper::use_chrdefbuilder(runtime_config & cfg)
{
	return (! cfg.chrdef_bpp.empty()) || (! cfg.chrdef_width.empty()) || (! cfg.chrdef_height.empty()) ||
				 (! cfg.chrdef_pixel_offsets.empty()) || (! cfg.chrdef_plane_offsets.empty()) ||
				 (! cfg.chrdef_row_offsets.empty());
}

bool def_helper::use_rgbcoldefbuilder(runtime_config & cfg)
{
	return (! cfg.rgbcoldef_big_endian.empty()) || (! cfg.rgbcoldef_rgblayout.empty()) ||
				 (! cfg.rgbcoldef_bitdepth.empty());
}

def_helper::def_helper(runtime_config & cfg) :
		m_defs {load_gfxdefs(cfg.gfxdefs_path)}
{

	// identify defs by id
	string chrdef_id, coldef_id, paldef_id;

	// configure from gfxprofile if specified
	if (! cfg.profile.empty())
	{
		auto profile = find_gfxprofile(cfg.profile);
		chrdef_id = profile.chrdef_id();
		coldef_id = profile.coldef_id();
		paldef_id = profile.paldef_id();
	}

	// specific gfxdefs will override profile settings
	if (! cfg.chrdef_id.empty())
		chrdef_id = cfg.chrdef_id;
	if (! cfg.coldef_id.empty())
		coldef_id = cfg.coldef_id;
	if (! cfg.paldef_id.empty())
		paldef_id = cfg.paldef_id;

	// load the requested defs
	if (! chrdef_id.empty())
		chrdef = find_chrdef(chrdef_id);
	if (! coldef_id.empty())
		coldef = find_coldef(coldef_id);
	if (! paldef_id.empty())
		paldef = find_paldef(paldef_id);

	// build def(s) from scratch or override parts of the loaded def(s)
	if (use_chrdefbuilder(cfg))
	{
		chrdef_builder builder(*chrdef);
		builder.from_chrdef(*chrdef);
		builder.set_id("chrdef_cli_generated");
		builder.set_desc("CHRDEF generated from CLI");
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
		m_heapallocated_chrdef = true;
		chrdef = new chrgfx::chrdef(builder.build());
	}

	if (use_rgbcoldefbuilder(cfg))
	{
		rgbcoldef_builder builder;
		if (coldef->type() == coldef_type::rgb)
			builder.from_rgbcoldef(*static_cast<rgbcoldef const *>(coldef));
		builder.set_id("rgbcoldef_cli_generated");
		builder.set_desc("COLDEF generated from CLI");
		if (! cfg.rgbcoldef_bitdepth.empty())
			builder.set_bitdepth(cfg.rgbcoldef_bitdepth);
		if (! cfg.rgbcoldef_rgblayout.empty())
			builder.set_layout(cfg.rgbcoldef_rgblayout);
		if (! cfg.rgbcoldef_big_endian.empty())
			builder.set_big_endian(cfg.rgbcoldef_big_endian);
		m_heapallocated_coldef = true;
		coldef = new chrgfx::rgbcoldef(builder.build());
	}

#ifdef DEBUG
	cerr << "\tUsing chrdef '" << this->chrdef->id() << "'\n";
	cerr << "\tUsing coldef '" << this->coldef->id() << "'\n";
	cerr << "\tUsing paldef '" << this->paldef->id() << "'\n";
#endif
}

def_helper::~def_helper()
{
	if (m_heapallocated_coldef)
		delete coldef;
	if (m_heapallocated_paldef)
		delete paldef;
	if (m_heapallocated_chrdef)
		delete chrdef;
}

void def_helper::list_gfxdefs(ostream & os)
{

	for (auto const & profile : m_defs.profiles)
	{
		os << "profile " << profile.second.id();
		if (! profile.second.description().empty())
			os << " (" << profile.second.description() << ')';
		os << " [External]\n";
	}

	for (auto const & chrdef : chrgfx::gfxdefs::chrdefs)
	{
		os << "chrdef " << chrdef.second.id();
		if (! chrdef.second.description().empty())
			os << " (" << chrdef.second.description() << ')';
		os << " [Internal]\n";
	}

	for (auto const & chrdef : m_defs.chrdefs)
	{
		os << "chrdef " << chrdef.second.id();
		if (! chrdef.second.description().empty())
			os << " (" << chrdef.second.description() << ')';
		os << " [External]\n";
	}

	for (auto const & paldef : chrgfx::gfxdefs::paldefs)
	{
		os << "paldef " << paldef.second.id();
		if (! paldef.second.description().empty())
			os << " (" << paldef.second.description() << ')';
		os << " [Internal]\n";
	}

	for (auto const & paldef : m_defs.paldefs)
	{
		os << "paldef " << paldef.second.id();
		if (! paldef.second.description().empty())
			os << " (" << paldef.second.description() << ')';
		os << " [External]\n";
	}

	for (auto const & rgbcoldef : chrgfx::gfxdefs::rgbcoldefs)
	{
		os << "coldef " << rgbcoldef.second.id();
		if (! rgbcoldef.second.description().empty())
			os << " (" << rgbcoldef.second.description() << ')';
		os << " [Internal]\n";
	}

	for (auto const & rgbcoldef : m_defs.rgbcoldefs)
	{
		os << "coldef " << rgbcoldef.second.id();
		if (! rgbcoldef.second.description().empty())
			os << " (" << rgbcoldef.second.description() << ')';
		os << " [External]\n";
	}

	for (auto const & refcoldef : m_defs.refcoldefs)
	{
		os << "coldef " << refcoldef.second.id();
		if (! refcoldef.second.description().empty())
			os << " (" << refcoldef.second.description() << ')';
		os << " [External]\n";
	}
}

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
			cfg.profile = optarg;
			break;

		case 'a':
			cfg.list_gfxdefs = true;
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
