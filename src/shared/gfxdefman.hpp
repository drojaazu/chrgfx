#ifndef __MOTOI__GFXDEFMAN_HPP
#define __MOTOI__GFXDEFMAN_HPP

#include "builtin_defs.hpp"
#include "cfgload.hpp"
#include "chrdef.hpp"
#include "coldef.hpp"
#include "gfxdef_builder.hpp"
#include "paldef.hpp"
#include "shared.hpp"
#include <map>
#include <vector>

struct gfxdef_config
{
	std::string gfxdefs_path;
	std::string profile_id;
	std::string chrdef_id;
	std::string coldef_id;
	std::string paldef_id;
};

struct chrdef_build_config
{
	std::string chrdef_width;
	std::string chrdef_height;
	std::string chrdef_bpp;
	std::string chrdef_plane_offsets;
	std::string chrdef_pixel_offsets;
	std::string chrdef_row_offsets;
};

struct rgbcoldef_build_config
{
	std::string rgbcoldef_big_endian;
	std::string rgbcoldef_rgblayout;
	std::string rgbcoldef_bitdepth;
};

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

	gfxdef_config const * m_gfxdef_cfg {nullptr};
	chrdef_build_config const * m_chrdef_build_cfg {nullptr};
	rgbcoldef_build_config const * m_rgbcoldef_build_cfg {nullptr};

	std::vector<chrgfx::gfxdef const *> m_allocated_gfxdefs;

	std::vector<std::string_view> m_errors;

	static auto constexpr GFXDEF_SUBDIR {"chrgfx/gfxdefs"};

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

			if (! cfg.chrdef_id.empty() && m_chrdef == nullptr && block_header == "chrdef")
			{
				auto kv = block.second.find("id");
				if (kv == block.second.end())
					throw runtime_error("no id found in chrdef block");

				if (kv->second != cfg.chrdef_id)
					continue;

				// matched the block, now load it as a gfxdef
				chrdef_builder builder(block.second);
				m_chrdef = builder.build();
				m_allocated_gfxdefs.push_back(m_chrdef);
				continue;
			}

			if (! cfg.paldef_id.empty() && m_paldef == nullptr && block_header == "paldef")
			{
				auto kv = block.second.find("id");
				if (kv == block.second.end())
					throw runtime_error("no id found in paldef block");

				if (kv->second != cfg.paldef_id)
					continue;

				// matched the block, now load it as a gfxdef
				paldef_builder builder(block.second);
				m_paldef = builder.build();
				m_allocated_gfxdefs.push_back(m_paldef);
				continue;
			}

			if (! cfg.coldef_id.empty() && m_coldef == nullptr && block_header == "rgbcoldef")
			{
				auto kv = block.second.find("id");
				if (kv == block.second.end())
					throw runtime_error("no id found in coldef block");

				if (kv->second != cfg.coldef_id)
					continue;

				// matched the block, now load it as a gfxdef
				rgbcoldef_builder builder(block.second);
				m_coldef = builder.build();
				m_allocated_gfxdefs.push_back(m_coldef);
				continue;
			}

			if (! cfg.coldef_id.empty() && m_coldef == nullptr && block_header == "refcoldef")
			{
				auto kv = block.second.find("id");
				if (kv == block.second.end())
					throw runtime_error("no id found in coldef block");

				if (kv->second != cfg.coldef_id)
					continue;

				// matched the block, now load it as a gfxdef
				refcoldef_builder builder(block.second);
				m_coldef = builder.build();
				m_allocated_gfxdefs.push_back(m_coldef);
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
				m_chrdef = &def->second;
		}

		if (m_paldef == nullptr && ! cfg.paldef_id.empty())
		{
			auto def = chrgfx::gfxdefs::paldefs.find(cfg.paldef_id);
			if (def != chrgfx::gfxdefs::paldefs.end())
				m_paldef = &def->second;
		}

		if (m_coldef == nullptr && ! cfg.coldef_id.empty())
		{
			auto def = chrgfx::gfxdefs::rgbcoldefs.find(cfg.coldef_id);
			if (def != chrgfx::gfxdefs::rgbcoldefs.end())
				m_coldef = &def->second;
		}
	}

public:
	~gfxdef_manager()
	{
		for (auto p : m_allocated_gfxdefs)
			delete p;
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

		load_from_file(cfg);
		load_from_internal(cfg);
	}
};

#endif