#ifndef CHRGFX__GFXDEF_BUILDER_HPP
#define CHRGFX__GFXDEF_BUILDER_HPP

#include <stdexcept>
#include <string>

#include "chrdef.hpp"
#include "coldef.hpp"
#include "paldef.hpp"
#include "strutil.hpp"

using namespace std;
using namespace chrgfx;

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

	void set_desc(string const & desc)
	{
		m_desc = desc;
	}
};

class chrdef_builder : public gfxdef_builder
{
private:
	uint m_width;
	uint m_height;
	uint m_bitdepth;
	vector<uint> m_plane_offsets;
	vector<uint> m_pixel_offsets;
	vector<uint> m_row_offsets;

public:
	chrdef_builder() = default;
	chrdef_builder(chrdef const & chrdef)
	{
		from_chrdef(chrdef);
	}

	void from_chrdef(chrdef const & chrdef)
	{
		set_id(chrdef.id());
		set_desc(chrdef.description());
		m_width = chrdef.width();
		m_height = chrdef.height();
		m_bitdepth = chrdef.bitdepth();
		m_plane_offsets = chrdef.plane_offsets();
		m_pixel_offsets = chrdef.pixel_offsets();
		m_row_offsets = chrdef.row_offsets();
	}

	void set_width(string const & width)
	{
		m_width = sto<uint>(width);
	}

	void set_height(string const & height)
	{
		m_height = sto<uint>(height);
	}

	void set_bitdepth(string const & bitdepth)
	{
		m_bitdepth = sto<uint>(bitdepth);
	}

	void set_plane_offsets(string const & plane_offsets)
	{
		if (plane_offsets[0] == '[')
			m_plane_offsets = sto_range_by_count<vector<uint>>(plane_offsets);
		else
			m_plane_offsets = sto_container<vector<uint>>(plane_offsets);
	}

	void set_pixel_offsets(string const & pixel_offsets)
	{
		if (pixel_offsets[0] == '[')
			m_pixel_offsets = sto_range_by_count<vector<uint>>(pixel_offsets);
		else
			m_pixel_offsets = sto_container<vector<uint>>(pixel_offsets);
	}

	void set_row_offsets(string const & row_offsets)
	{
		if (row_offsets[0] == '[')
			m_row_offsets = sto_range_by_count<vector<uint>>(row_offsets);
		else
			m_row_offsets = sto_container<vector<uint>>(row_offsets);
	}

	[[nodiscard]] chrdef build() const
	{
		return {m_id, m_width, m_height, m_bitdepth, m_plane_offsets, m_pixel_offsets, m_row_offsets, m_desc};
	}
};

#endif
