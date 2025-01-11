#ifndef __MOTOI__GFXDEFMAN_HPP
#define __MOTOI__GFXDEFMAN_HPP

#include "builtin_defs.hpp"
#include "cfgload.hpp"
#include "chrdef.hpp"
#include "coldef.hpp"
#include "gfxdef_builder.hpp"
#include "paldef.hpp"
#include "shared.hpp"
#include "xdgdirs.hpp"
#include <map>
#include <vector>

static auto constexpr GFXDEF_SUBDIR {"chrgfx/gfxdefs"};

std::string get_gfxdefs_path()
{
	auto xdg_locations {data_filepaths(GFXDEF_SUBDIR)};
	if (xdg_locations.empty())
		throw runtime_error("Could not find gfxdef file in any default location");
	return xdg_locations.front();
}

class gfxdef_manager
{
private:
	chrgfx::chrdef const * m_chrdef {nullptr};
	chrgfx::paldef const * m_paldef {nullptr};
	chrgfx::coldef const * m_coldef {nullptr};

	std::string m_target_profile;
	std::string m_target_chrdef;
	std::string m_target_paldef;
	std::string m_target_coldef;

	std::vector<std::string_view> m_errors;

	enum class def_type
	{
		chrdef,
		paldef,
		coldef,
		profile
	};

	// clang-format off
std::map<std::string_view, def_type> const def_type_lexemes {
	{"chrdef", def_type::chrdef},
	{"paldef", def_type::paldef},
	{"coldef", def_type::coldef},
	{"profile", def_type::profile}
};
	// clang-format on

	void load_from_file(runtime_config & cfg)
	{
		// don't even bother loading anything if we already have our defs set up
		if (m_chrdef != nullptr && m_paldef != nullptr && m_coldef != nullptr)
			return;

		motoi::config_loader config(cfg.gfxdefs_path);

		std::string_view block_header;
		// get gfxdef IDs from profile first, if specified
		if (! cfg.profile_id.empty())
		{
			for (auto const & block : config)
			{
				if (block.first != "profile")
					continue;

				auto kv = block.second.find("id");
				if (kv == block.second.end())
					throw runtime_error("no id found in profile block");
				if (kv->second != cfg.profile_id)
					continue;

				// we have our requested profile
				// only set gfxdef IDs if they are not set by the user
				// (allow them to override profile settings)
				if (cfg.chrdef_id.empty())
				{
					auto chrdef_kv = block.second.find("chrdef");
					if (chrdef_kv != block.second.end())
						cfg.chrdef_id = chrdef_kv->second;
				}

				if (cfg.paldef_id.empty())
				{
					auto paldef_kv = block.second.find("paldef");
					if (paldef_kv != block.second.end())
						cfg.paldef_id = paldef_kv->second;
				}

				if (cfg.coldef_id.empty())
				{
					auto coldef_kv = block.second.find("coldef");
					if (coldef_kv != block.second.end())
						cfg.coldef_id = coldef_kv->second;
				}
				break;
			}
		}

		// load gfxdefs as specified
		for (auto const & block : config)
		{
			block_header = block.first;

			if (m_chrdef == nullptr && ! cfg.chrdef_id.empty() && block_header == "chrdef")
			{
				auto kv = block.second.find("id");
				if (kv == block.second.end())
					continue;

				if (kv->second != cfg.chrdef_id)
					continue;

				// matched the block, now load it as a gfxdef
				chrdef_builder builder(block.second);
				m_chrdef = builder.build();
				continue;
			}

			if (m_paldef == nullptr && ! cfg.paldef_id.empty() && block_header == "paldef")
			{
				auto kv = block.second.find("id");
				if (kv == block.second.end())
					continue;

				if (kv->second != cfg.paldef_id)
					continue;

				// matched the block, now load it as a gfxdef
				paldef_builder builder(block.second);
				m_paldef = builder.build();
				continue;
			}

			if (m_coldef == nullptr && ! cfg.coldef_id.empty() && block_header == "rgbcoldef")
			{
				auto kv = block.second.find("id");
				if (kv == block.second.end())
					continue;

				if (kv->second != cfg.coldef_id)
					continue;

				// matched the block, now load it as a gfxdef
				rgbcoldef_builder builder(block.second);
				m_coldef = builder.build();
				continue;
			}

			if (m_coldef == nullptr && ! cfg.coldef_id.empty() && block_header == "refcoldef")
			{
				auto kv = block.second.find("id");
				if (kv == block.second.end())
					continue;

				if (kv->second != cfg.coldef_id)
					continue;

				// matched the block, now load it as a gfxdef
				refcoldef_builder builder(block.second);
				m_coldef = builder.build();
				continue;
			}
		}
	}

	void load_from_internal(runtime_config & cfg)
	{
		if (m_chrdef == nullptr && ! cfg.chrdef_id.empty())
		{
			auto def = chrgfx::gfxdefs::chrdefs.find(cfg.chrdef_id);
			if (def != chrgfx::gfxdefs::chrdefs.end())
				m_chrdef = new class chrdef(def->second);
		}

		if (m_paldef == nullptr && ! cfg.paldef_id.empty())
		{
			auto def = chrgfx::gfxdefs::paldefs.find(cfg.paldef_id);
			if (def != chrgfx::gfxdefs::paldefs.end())
				m_paldef = new class paldef(def->second);
		}

		if (m_coldef == nullptr && ! cfg.coldef_id.empty())
		{
			auto def = chrgfx::gfxdefs::rgbcoldefs.find(cfg.coldef_id);
			if (def != chrgfx::gfxdefs::rgbcoldefs.end())
				m_coldef = new class rgbcoldef(def->second);
		}
	}

	void load_from_cli(runtime_config & cfg)
	{
		// build chrdef from cli
		if (cfg.chrdef_cli_defined())
		{
			chrdef_builder builder;
			if (m_chrdef != nullptr)
			{
				builder.from_def(*m_chrdef);
				delete m_chrdef;
			}
			if (! cfg.chrdef_bpp.empty())
				builder.set_bitdepth(cfg.chrdef_bpp);
			if (! cfg.chrdef_width.empty())
				builder.set_width(cfg.chrdef_width);
			if (! cfg.chrdef_height.empty())
				builder.set_height(cfg.chrdef_height);
			if (! cfg.chrdef_pixel_offsets.empty())
				builder.set_pixel_offsets(cfg.chrdef_pixel_offsets);
			if (! cfg.chrdef_plane_offsets.empty())
				builder.set_plane_offsets(cfg.chrdef_plane_offsets);
			if (! cfg.chrdef_row_offsets.empty())
				builder.set_row_offsets(cfg.chrdef_row_offsets);

			m_chrdef = builder.build();
		}

		// build paldef from cli
		if (cfg.paldef_cli_defined())
		{
			paldef_builder builder;
			if (m_paldef != nullptr)
			{
				builder.from_def(*m_paldef);
				delete m_paldef;
			}
			if (! cfg.paldef_datasize.empty())
				builder.set_datasize(cfg.paldef_datasize);
			if (! cfg.paldef_entry_datasize.empty())
				builder.set_datasize(cfg.paldef_entry_datasize);
			if (! cfg.paldef_length.empty())
				builder.set_length(cfg.paldef_length);

			m_paldef = builder.build();
		}

		// build coldef from cli
		if (cfg.coldef_cli_defined())
		{
			rgbcoldef_builder builder;
			if (m_coldef != nullptr)
			{
				if (m_coldef->type() == coldef_type::rgb)
					builder.from_def(*static_cast<rgbcoldef const *>(m_coldef));
				delete m_coldef;
			}
			if (! cfg.rgbcoldef_bitdepth.empty())
				builder.set_bitdepth(cfg.rgbcoldef_bitdepth);
			if (! cfg.rgbcoldef_big_endian.empty())
				builder.set_big_endian(cfg.rgbcoldef_big_endian);
			if (! cfg.rgbcoldef_rgblayout.empty())
				builder.set_layout(cfg.rgbcoldef_rgblayout);

			m_coldef = builder.build();
		}
	}

public:
	~gfxdef_manager()
	{
		delete m_chrdef;
		delete m_paldef;
		delete m_coldef;
	}

	auto chrdef()
	{
		return m_chrdef;
	}

	auto paldef()
	{
		return m_paldef;
	}

	auto coldef()
	{
		return m_coldef;
	}

	void load_gfxdefs(runtime_config & cfg)
	{
		// if no IDs are specified (i.e. building entirely from command line), skip these steps
		if (! (cfg.profile_id.empty() && cfg.chrdef_id.empty() && cfg.coldef_id.empty() && cfg.paldef_id.empty()))
		{
			// we load from file first to get the profile, if specified
			load_from_file(cfg);
			load_from_internal(cfg);
		}
		load_from_cli(cfg);
	}
};

#endif