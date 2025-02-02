/**
 * @file gfxdef_builder.hpp
 * @author Motoi Productions (Damian Rogers damian@motoi.pro)
 * @brief Creates a gfxdef on the fly
 */

#ifndef __CHRGFX__GFXDEF_BUILDER_HPP
#define __CHRGFX__GFXDEF_BUILDER_HPP

#include <optional>
#include <stdexcept>
#include <string>

#include "cfgload.hpp"
#include "chrdef.hpp"
#include "coldef.hpp"
#include "image_types.hpp"
#include "paldef.hpp"
#include "strutil.hpp"

using namespace std;
using namespace chrgfx;
using namespace motoi;

#define SET_FIELD(_field) \
	if (entry.first == #_field) \
	{ \
		set_##_field(entry.second); \
		continue; \
	}

/*
we may add builders for paldef/coldef sometime in the future, so we have an abstract class

however, coldefs in particular are a bit more complicated than chrdefs when it comes to command line definitions

for example, for rgbcoldefs, if we override an existing loaded def (e.g. from a profile) with a color passes value that
does not match the overridden def, we have a problem

moreover, trying to specify a refcoldef on the command line sounds like a nightmare

all that aside, I can't think of a use case where col/pal defs would need to be modified "on the fly" like chrdefs may
need to be (e.g. large "tiles" with non-uniform sizes)
*/

class gfxdef_builder
{
protected:
	string m_id;
	string m_desc;
	gfxdef_builder() {}

public:
	void set_id(string const & id)
	{
		if (id.empty())
			throw runtime_error("gfxdef id cannot be empty");
		m_id = id;
	}

	void set_desc(string const & comment)
	{
		m_desc = comment;
	}
};

class rgbcoldef_builder : public gfxdef_builder
{
private:
	uint m_bitdepth {0};
	std::vector<rgb_layout> m_layout;
	bool m_big_endian {false};

public:
	rgbcoldef_builder() = default;
	rgbcoldef_builder(rgbcoldef const & coldef)
	{
		from_def(coldef);
	}

	rgbcoldef_builder(block_map const & map)
	{
		from_map(map);
	}

	void from_def(rgbcoldef const & coldef)
	{
		set_id(coldef.id());
		set_desc(coldef.desc());
		m_bitdepth = coldef.bitdepth();
		m_layout = coldef.layout();
	}

	void from_map(block_map const & map)
	{
		for (auto const & entry : map)
		{
			SET_FIELD(id);
			SET_FIELD(desc);
			SET_FIELD(bitdepth);
			SET_FIELD(layout);
			SET_FIELD(big_endian);
		}
	}

	void set_bitdepth(string const & bitdepth)
	{
		m_bitdepth = sto<uint>(trim_view(bitdepth));
	}

	void clear_layout()
	{
		m_layout.clear();
	}

	void set_layout(string const & layout)
	{
		auto layout_raw = sto_container<vector<uint>>(layout);
		if (layout_raw.size() != 6)
			throw runtime_error("invalid rgb layout for coldef, must have exactly 6 entries");
		rgb_layout rgblayout {
			{layout_raw[0], layout_raw[1]}, {layout_raw[2], layout_raw[3]}, {layout_raw[4], layout_raw[5]}};
		m_layout.emplace_back(rgblayout);
	}

	void set_big_endian(string const & big_endian)
	{
		m_big_endian = sto_bool(big_endian);
	}

	[[nodiscard]] rgbcoldef * build() const
	{
		// check the validity of the definition
		if (m_bitdepth == 0)
			throw runtime_error("Bitdepth must be greater than zero");
		if (m_layout.size() == 0)
			throw runtime_error("rgb_layout list cannot be empty");
		for (auto const & layout : m_layout)
		{
			if (layout.red_size() > 8 || layout.green_size() > 8 || layout.blue_size() > 8)
				throw runtime_error("datasize in a bit layout cannot be greater than 8 (maximum of 8 bits per color channel)");
			if (layout.red_offset() > 32 || layout.green_offset() > 32 || layout.blue_offset() > 32)
				throw runtime_error("color channel bit offsets cannot be greater than 32");
		}
		return new rgbcoldef {m_id, m_bitdepth, m_layout, m_big_endian, m_desc};
	}
};

class refcoldef_builder : public gfxdef_builder
{
private:
	palette m_refpal;
	bool m_big_endian;

public:
	refcoldef_builder() = default;

	refcoldef_builder(refcoldef const & coldef)
	{
		from_def(coldef);
	}

	refcoldef_builder(block_map const & map)
	{
		from_map(map);
	}

	void from_def(refcoldef const & coldef)
	{
		set_id(coldef.id());
		set_desc(coldef.desc());
		m_refpal = coldef.refpal();
	}

	void from_map(block_map const & map)
	{
		for (auto const & entry : map)
		{
			SET_FIELD(id);
			SET_FIELD(desc);
			SET_FIELD(big_endian);
			SET_FIELD(refpal);
		}
	}

	void set_refpal(string const & refpal)
	{
		m_refpal = split_array<rgb_color, 256>(refpal);
	}

	void set_big_endian(string const & big_endian)
	{
		m_big_endian = sto_bool(trim_view(big_endian));
	}

	[[nodiscard]] refcoldef * build() const
	{
		return new refcoldef {m_id, m_refpal, m_big_endian, m_desc};
	}
};

class paldef_builder : public gfxdef_builder
{
private:
	uint m_entry_datasize {0};
	uint m_length {0};
	std::optional<uint> m_datasize {std::nullopt};

public:
	paldef_builder() = default;
	paldef_builder(paldef const & paldef)
	{
		from_def(paldef);
	}

	paldef_builder(block_map const & map)
	{
		from_map(map);
	}

	void from_def(paldef const & paldef)
	{
		set_id(paldef.id());
		set_desc(paldef.desc());
		m_entry_datasize = paldef.entry_datasize();
		m_length = paldef.length();
		m_datasize = paldef.datasize();
	}

	void from_map(block_map const & map)
	{
		for (auto const & entry : map)
		{
			SET_FIELD(id);
			SET_FIELD(desc);
			SET_FIELD(length);
			SET_FIELD(entry_datasize);
			SET_FIELD(datasize);
		}
	}

	void set_length(string const & length)
	{
		m_length = sto<uint>(length);
	}

	void set_entry_datasize(string const & entry_datasize)
	{
		m_entry_datasize = sto<uint>(trim_view(entry_datasize));
	}

	void set_datasize(string const & datasize)
	{
		m_datasize = sto<uint>(trim_view(datasize));
	}

	[[nodiscard]] paldef * build() const
	{
		// check the validity of the definition
		if (m_length == 0)
			throw runtime_error("palette length must be greater than zero");
		if (m_entry_datasize == 0)
			throw runtime_error("palette entry data size must be greater than zero");
		if (m_datasize == 0)
			throw runtime_error("palette data size must be greater than zero");

		return new paldef {m_id, m_entry_datasize, m_length, m_datasize, m_desc};
	}
};

class chrdef_builder : public gfxdef_builder
{
private:
	uint m_width {0};
	uint m_height {0};
	uint m_bpp {0};
	vector<uint> m_plane_offsets;
	vector<uint> m_pixel_offsets;
	vector<uint> m_row_offsets;

public:
	chrdef_builder() = default;
	chrdef_builder(chrdef const & chrdef)
	{
		from_def(chrdef);
	}

	chrdef_builder(block_map const & map)
	{
		from_map(map);
	}

	void from_def(chrdef const & chrdef)
	{
		set_id(chrdef.id());
		set_desc(chrdef.desc());
		m_width = chrdef.width();
		m_height = chrdef.height();
		m_bpp = chrdef.bpp();
		m_plane_offsets = chrdef.plane_offsets();
		m_pixel_offsets = chrdef.pixel_offsets();
		m_row_offsets = chrdef.row_offsets();
	}

	void from_map(block_map const & map)
	{
		for (auto const & entry : map)
		{
			SET_FIELD(id);
			SET_FIELD(desc);
			SET_FIELD(width);
			SET_FIELD(height);
			SET_FIELD(bpp);
			SET_FIELD(plane_offsets);
			SET_FIELD(pixel_offsets);
			SET_FIELD(row_offsets);
		}
	}

	void set_width(string const & width)
	{
		m_width = sto<uint>(trim_view(width));
	}

	void set_height(string const & height)
	{
		m_height = sto<uint>(trim_view(height));
	}

	void set_bpp(string const & bpp)
	{
		m_bpp = sto<uint>(trim_view(bpp));
	}

	void set_plane_offsets(string const & plane_offsets)
	{
		if (plane_offsets[0] == '[')
			m_plane_offsets = sto_range<vector<uint>>(plane_offsets);
		else
			m_plane_offsets = sto_container<vector<uint>>(plane_offsets);
	}

	void set_pixel_offsets(string const & pixel_offsets)
	{
		if (pixel_offsets[0] == '[')
			m_pixel_offsets = sto_range<vector<uint>>(pixel_offsets);
		else
			m_pixel_offsets = sto_container<vector<uint>>(pixel_offsets);
	}

	void set_row_offsets(string const & row_offsets)
	{
		if (row_offsets[0] == '[')
			m_row_offsets = sto_range<vector<uint>>(row_offsets);
		else
			m_row_offsets = sto_container<vector<uint>>(row_offsets);
	}

	[[nodiscard]] chrdef * build() const
	{
		// check the validity of the definition
		if (m_bpp == 0)
			throw runtime_error("Bitdepth must be greater than zero");
		if (m_width > m_pixel_offsets.size())
			throw runtime_error("CHR width must be equal to number of pixel offset entries");
		if (m_height > m_row_offsets.size())
			throw runtime_error("CHR height must be equal to number of row offset entries");
		if (m_bpp > m_plane_offsets.size())
			throw runtime_error("CHR bitdepth must be equal to number of plane offset entries");
		return new chrdef {m_id, m_width, m_height, m_bpp, m_pixel_offsets, m_row_offsets, m_plane_offsets, m_desc};
	}
};

#endif
