#include "shared.hpp"

using namespace std;

string const CONFIG_PATH {"/etc/chrgfx"};
string const GFXDEF_PATH {CONFIG_PATH + "/gfxdefs"};

runtime_config::runtime_config () :
		list_gfxdefs (false) {};

def_helper::def_helper (runtime_config & cfg) :
		m_defs (load_gfxdefs (cfg.gfxdefs_path.empty () ? GFXDEF_PATH : cfg.gfxdefs_path))
{
	if (cfg.list_gfxdefs)
	{
		list_gfxdefs (cout);
		exit (0);
	}

	string chrdef_id, coldef_id, paldef_id;

	// configure from gfxprofile if specified
	if (! cfg.profile.empty ())
	{
		auto i_profile {m_defs.profiles.find (cfg.profile)};
		if (i_profile == m_defs.profiles.end ())
			throw invalid_argument ("Could not find specified profile");

		chrdef_id = i_profile->second.chrdef_id ();
		coldef_id = i_profile->second.coldef_id ();
		paldef_id = i_profile->second.paldef_id ();
	}

	// specific gfxdefs override profile settings
	if (! cfg.chrdef_id.empty ())
		chrdef_id = cfg.chrdef_id;

	if (! cfg.coldef_id.empty ())
		coldef_id = cfg.coldef_id;

	if (! cfg.paldef_id.empty ())
		paldef_id = cfg.paldef_id;

	// chrdef is a requirement
	if (chrdef_id.empty ())
		throw invalid_argument ("No tile encoding specified");

	// use built-in coldef if not specified
	if (coldef_id.empty ())
		coldef_id = "basic_8bit_random";

	// use built-in paldef is not specified
	if (paldef_id.empty ())
		paldef_id = "basic_256color";

	auto i_find_chrdef {m_defs.chrdefs.find (chrdef_id)};
	if (i_find_chrdef == m_defs.chrdefs.end ())
		throw invalid_argument ("Could not find specified tile encoding (chrdef)");
	chrdef = &i_find_chrdef->second;

	auto i_find_rgbcoldef {m_defs.rgbcoldefs.find (coldef_id)};
	if (i_find_rgbcoldef != m_defs.rgbcoldefs.end ())
	{
		coldef = &i_find_rgbcoldef->second;
	}
	else
	{
		auto i_find_refcoldef {m_defs.refcoldefs.find (coldef_id)};
		if (i_find_refcoldef != m_defs.refcoldefs.end ())
		{
			coldef = &i_find_refcoldef->second;
		}
		else
		{
			throw invalid_argument ("Could not find specified color encoding (coldef)");
		}
	}

	auto i_find_paldef {m_defs.paldefs.find (paldef_id)};
	if (i_find_paldef == m_defs.paldefs.end ())
		throw invalid_argument ("Could not find specified palette encoding (paldef)");
	paldef = &i_find_paldef->second;
}

void def_helper::list_gfxdefs (ostream & os)
{
	for (auto const & profile : m_defs.profiles)
	{
		os << "profile " << profile.second.id ();
		if (! profile.second.description ().empty ())
			os << " (" << profile.second.description () << ')';
		os << endl;
	}

	for (auto const & chrdef : m_defs.chrdefs)
	{
		os << "chrdef " << chrdef.second.id ();
		if (! chrdef.second.description ().empty ())
			os << " (" << chrdef.second.description () << ')';
		os << endl;
	}

	for (auto const & paldef : m_defs.paldefs)
	{
		os << "paldef " << paldef.second.id ();
		if (! paldef.second.description ().empty ())
			os << " (" << paldef.second.description () << ')';
		os << endl;
	}

	for (auto const & rgbcoldef : m_defs.rgbcoldefs)
	{
		os << "coldef " << rgbcoldef.second.id ();
		if (! rgbcoldef.second.description ().empty ())
			os << " (" << rgbcoldef.second.description () << ')';
		os << endl;
	}
	for (auto const & refcoldef : m_defs.refcoldefs)
	{
		os << "coldef " << refcoldef.second.id ();
		if (! refcoldef.second.description ().empty ())
			os << " (" << refcoldef.second.description () << ')';
		os << endl;
	}
}

// command line argument processing
string short_opts {":G:P:T:C:L:lhv"};

vector<option> long_opts {{"gfx-def", required_argument, nullptr, 'G'},
	{"profile", required_argument, nullptr, 'P'},
	{"chr-def", required_argument, nullptr, 'T'},
	{"col-def", required_argument, nullptr, 'C'},
	{"pal-def", required_argument, nullptr, 'L'},
	{"list-gfxdefs", no_argument, nullptr, 'l'},
	{"help", no_argument, nullptr, 'h'},
	{"version", no_argument, nullptr, 'v'}};

vector<option_details> opt_details {
	{false, L"Path to graphics encoding definitions", L"path"},
	{false, L"Graphics profile to use", nullptr},
	{false,
		L"Tile encoding to use; overrides tile encoding in graphics profile "
		L"(if specified)",
		nullptr},
	{false,
		L"Color encoding to use; overrides color encoding in graphics "
		L"profile (if specified)",
		nullptr},
	{false,
		L"Palette encoding to use; overrides palette encoding in "
		L"graphics profile (if specified)",
		nullptr},
	{false, L"List all available encodings in gfxdefs file", nullptr},
	{false, L"Display program usage", nullptr},
	{false, L"Display program version", nullptr},
};

bool shared_args (char this_opt, runtime_config & cfg)
{

	bool found = true;

	switch (this_opt)
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

		case 'l':
			cfg.list_gfxdefs = true;
			break;

		case 'h':
			show_usage (long_opts.data (), opt_details.data (), wcout);
			exit (0);

		case 'v':
			show_version (wcout);
			exit (0);

		case ':':
			cerr << "Missing arg for option: " << to_string (optopt) << endl;
			exit (-1);

		case '?':
			cerr << "Unknown argument" << endl;
			exit (-2);

		default:
			found = false;
	}

	return found;
}
