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
#ifdef DEBUG
#include <iostream>
#endif

static auto constexpr GFXDEF_SUBDIR {"chrgfx/gfxdefs"};

class gfxdef_manager
{
private:
	runtime_config const & m_cfg;

	chrgfx::chrdef const * m_chrdef {nullptr};
	chrgfx::paldef const * m_paldef {nullptr};
	chrgfx::coldef const * m_coldef {nullptr};

	std::string m_target_profile;
	std::string m_target_chrdef;
	std::string m_target_paldef;
	std::string m_target_coldef;

	bool profile_found {false};

	std::string get_gfxdefs_xdg_paths()
	{
		auto xdg_locations {data_filepaths(GFXDEF_SUBDIR)};
		if (xdg_locations.empty())
			throw runtime_error("Could not find gfxdef file in any default location");
		return xdg_locations.front();
	}

	void load_from_file(std::string const & path)
	{
		// don't bother loading anything if we already have our defs loaded
		if (m_chrdef != nullptr && m_paldef != nullptr && m_coldef != nullptr)
		{
#ifdef DEBUG
			std::cerr << "gfxdef pointers alreadt set, not loading file\n";
#endif
			return;
		}

		motoi::config_loader config(path);
		std::string_view block_header;

		// get gfxdef IDs from profile first, if specified
		if (! m_target_profile.empty())
		{
#ifdef DEBUG
			std::cerr << "Using profile: " << m_target_profile << '\n';
#endif
			for (auto const & block : config)
			{
				if (block.first != "profile")
					continue;

				auto kv = block.second.find("id");
				if (kv == block.second.end())
					continue;
				if (kv->second != m_target_profile)
					continue;

				// we have our requested profile
				// only set gfxdef IDs if they are not set by the user
				// (i.e. allow them to override profile settings)
				profile_found = true;
				if (m_target_chrdef.empty())
				{
					auto chrdef_kv = block.second.find("chrdef");
					if (chrdef_kv != block.second.end())
						m_target_chrdef = chrdef_kv->second;
#ifdef DEBUG
					std::cerr << "Set target chrdef from profile: " << m_target_chrdef << '\n';
#endif
				}

				if (m_target_paldef.empty())
				{
					auto paldef_kv = block.second.find("paldef");
					if (paldef_kv != block.second.end())
						m_target_paldef = paldef_kv->second;
#ifdef DEBUG
					std::cerr << "Set target paldef from profile: " << m_target_paldef << '\n';
#endif
				}

				if (m_target_coldef.empty())
				{
					auto coldef_kv = block.second.find("coldef");
					if (coldef_kv != block.second.end())
						m_target_coldef = coldef_kv->second;
#ifdef DEBUG
					std::cerr << "Set target coldef from profile: " << m_target_coldef << '\n';
#endif
				}
				break;
			}

			if (! profile_found)
				throw runtime_error("could not find specified profile " + m_target_profile);
		}

		// load gfxdefs as specified
		for (auto const & block : config)
		{
			block_header = block.first;

			if (m_chrdef == nullptr && ! m_target_chrdef.empty() && block_header == "chrdef")
			{
				auto kv = block.second.find("id");
				if (kv == block.second.end())
					continue;

				if (kv->second != m_target_chrdef)
					continue;

				// matched the block, now load it as a gfxdef
				chrdef_builder builder(block.second);
				m_chrdef = builder.build();
				continue;
			}

			if (m_paldef == nullptr && ! m_target_paldef.empty() && block_header == "paldef")
			{
				auto kv = block.second.find("id");
				if (kv == block.second.end())
					continue;

				if (kv->second != m_target_paldef)
					continue;

				// matched the block, now load it as a gfxdef
				paldef_builder builder(block.second);
				m_paldef = builder.build();
				continue;
			}

			if (m_coldef == nullptr && ! m_target_coldef.empty() && block_header == "rgbcoldef")
			{
				auto kv = block.second.find("id");
				if (kv == block.second.end())
					continue;

				if (kv->second != m_target_coldef)
					continue;

				// matched the block, now load it as a gfxdef
				rgbcoldef_builder builder(block.second);
				m_coldef = builder.build();
				continue;
			}

			if (m_coldef == nullptr && ! m_target_coldef.empty() && block_header == "refcoldef")
			{
				auto kv = block.second.find("id");
				if (kv == block.second.end())
					continue;

				if (kv->second != m_target_coldef)
					continue;

				// matched the block, now load it as a gfxdef
				refcoldef_builder builder(block.second);
				m_coldef = builder.build();
				continue;
			}
		}
	}

	void load_from_internal()
	{
		if (m_chrdef == nullptr && ! m_target_chrdef.empty())
		{
			auto def = chrgfx::gfxdefs::chrdefs.find(m_target_chrdef);
			if (def != chrgfx::gfxdefs::chrdefs.end())
				m_chrdef = new class chrdef(def->second);
		}

		if (m_paldef == nullptr && ! m_target_paldef.empty())
		{
			auto def = chrgfx::gfxdefs::paldefs.find(m_target_paldef);
			if (def != chrgfx::gfxdefs::paldefs.end())
				m_paldef = new class paldef(def->second);
		}

		if (m_coldef == nullptr && ! m_target_coldef.empty())
		{
			auto def = chrgfx::gfxdefs::rgbcoldefs.find(m_target_coldef);
			if (def != chrgfx::gfxdefs::rgbcoldefs.end())
				m_coldef = new class rgbcoldef(def->second);
		}
	}

	void load_from_cli()
	{
		// build chrdef from cli
		if (m_cfg.chrdef_cli_defined())
		{
			chrdef_builder builder;
			if (m_chrdef != nullptr)
			{
				builder.from_def(*m_chrdef);
				delete m_chrdef;
			}
			if (! m_cfg.chrdef_bpp.empty())
				builder.set_bitdepth(m_cfg.chrdef_bpp);
			if (! m_cfg.chrdef_width.empty())
				builder.set_width(m_cfg.chrdef_width);
			if (! m_cfg.chrdef_height.empty())
				builder.set_height(m_cfg.chrdef_height);
			if (! m_cfg.chrdef_pixel_offsets.empty())
				builder.set_pixel_offsets(m_cfg.chrdef_pixel_offsets);
			if (! m_cfg.chrdef_plane_offsets.empty())
				builder.set_plane_offsets(m_cfg.chrdef_plane_offsets);
			if (! m_cfg.chrdef_row_offsets.empty())
				builder.set_row_offsets(m_cfg.chrdef_row_offsets);

			m_chrdef = builder.build();
		}

		// build paldef from cli
		if (m_cfg.paldef_cli_defined())
		{
			paldef_builder builder;
			if (m_paldef != nullptr)
			{
				builder.from_def(*m_paldef);
				delete m_paldef;
			}
			if (! m_cfg.paldef_datasize.empty())
				builder.set_datasize(m_cfg.paldef_datasize);
			if (! m_cfg.paldef_entry_datasize.empty())
				builder.set_datasize(m_cfg.paldef_entry_datasize);
			if (! m_cfg.paldef_length.empty())
				builder.set_length(m_cfg.paldef_length);

			m_paldef = builder.build();
		}

		// build coldef from cli
		if (m_cfg.coldef_cli_defined())
		{
			rgbcoldef_builder builder;
			if (m_coldef != nullptr)
			{
				if (m_coldef->type() == coldef_type::rgb)
					builder.from_def(*static_cast<rgbcoldef const *>(m_coldef));
				delete m_coldef;
			}
			if (! m_cfg.rgbcoldef_bitdepth.empty())
				builder.set_bitdepth(m_cfg.rgbcoldef_bitdepth);
			if (! m_cfg.rgbcoldef_big_endian.empty())
				builder.set_big_endian(m_cfg.rgbcoldef_big_endian);
			if (! m_cfg.rgbcoldef_rgblayout.empty())
				builder.set_layout(m_cfg.rgbcoldef_rgblayout);

			m_coldef = builder.build();
		}
	}

public:
	gfxdef_manager(runtime_config & cfg) :
			m_cfg(cfg),
			m_target_profile(m_cfg.profile_id),
			m_target_chrdef(m_cfg.chrdef_id),
			m_target_paldef(m_cfg.paldef_id),
			m_target_coldef(m_cfg.coldef_id)
	{
		// if no IDs are specified (i.e. building entirely from command line), skip these steps
		if (! (m_target_profile.empty() && m_target_chrdef.empty() && m_target_paldef.empty() && m_target_coldef.empty()))
		{
			// we load from file first to get the profile, if specified
			if (! m_cfg.gfxdefs_path.empty())
			{
				// if a gfxdefs file was specified, use that one only
				load_from_file(m_cfg.gfxdefs_path);
#ifdef DEBUG
				std::cerr << "Considering gfxdefs file: " << m_cfg.gfxdefs_path << '\n';
#endif
			}
			else
			{
				// otherwise read from XDG location(s)
				auto xdg_locations {motoi::data_filepaths(GFXDEF_SUBDIR)};
				if (xdg_locations.empty())
					throw runtime_error("could not find a gfxdefs file in any default location");
				for (auto const & path : xdg_locations)
				{
#ifdef DEBUG
					std::cerr << "Considering gfxdefs file: " << path << '\n';
#endif
					load_from_file(path);
				}
			}

			if (! m_target_profile.empty() && ! profile_found)
			{
				throw runtime_error("could not find specified profile " + m_target_profile);
			}

			load_from_internal();
			if (! m_target_chrdef.empty() && m_chrdef == nullptr)
				throw runtime_error("could not find specified chrdef " + m_target_chrdef);
			if (! m_target_paldef.empty() && m_paldef == nullptr)
				throw runtime_error("could not find specified paldef " + m_target_paldef);
			if (! m_target_coldef.empty() && m_coldef == nullptr)
				throw runtime_error("could not find specified coldef " + m_target_coldef);
		}
		load_from_cli();
	}

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
};

#endif