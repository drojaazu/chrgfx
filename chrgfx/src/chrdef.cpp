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
		m_datasize(width * height * bitdepth), m_planeoffset(planeoffset),
		m_pixeloffset(pixeloffset), m_rowoffset(rowoffset) {};

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
};

ushort chrdef::plane_offset(ushort const index) const
{
	return m_planeoffset[index];
};

ushort chrdef::pixel_offset(ushort const index) const
{
	return m_pixeloffset[index];
};

ushort chrdef::row_offset(ushort const index) const
{
	return m_rowoffset[index];
};

} // namespace chrgfx
