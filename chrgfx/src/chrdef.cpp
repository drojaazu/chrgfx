#include "chrdef.hpp"

using namespace std;

namespace chrgfx
{
chrdef::chrdef(string const & id, ushort const width, ushort const height,
							 ushort const bitdepth, vector<ushort> const & planeoffset,
							 vector<ushort> const & pixeloffset,
							 vector<ushort> const & rowoffset) :
		gfxdef(id),
		m_width(width), m_height(height), m_bitdepth(bitdepth),
		m_datasize(width * height * bitdepth), m_planeoffsets(planeoffset),
		m_pixeloffsets(pixeloffset), m_rowoffsets(rowoffset) {};

ushort chrdef::width() const
{
	return m_width;
}

ushort chrdef::height() const
{
	return m_height;
}

ushort chrdef::bitdepth() const
{
	return m_bitdepth;
}

ushort chrdef::datasize() const
{
	return m_datasize;
}

vector<ushort> const & chrdef::plane_offsets() const
{
	return m_planeoffsets;
}

vector<ushort> const & chrdef::pixel_offsets() const
{
	return m_pixeloffsets;
}

vector<ushort> const & chrdef::row_offsets() const
{
	return m_rowoffsets;
}

ushort chrdef::plane_offset_at(ushort const index) const
{
	return m_planeoffsets[index];
}

ushort chrdef::pixel_offset_at(ushort const index) const
{
	return m_pixeloffsets[index];
}

ushort chrdef::row_offset_at(ushort const index) const
{
	return m_rowoffsets[index];
}

} // namespace chrgfx
