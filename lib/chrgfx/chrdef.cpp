#include "chrdef.hpp"

using namespace std;

namespace chrgfx
{
chrdef::chrdef(string const & id,
	uint const width,
	uint const height,
	uint const bitdepth,
	vector<uint> const & pixeloffset,
	vector<uint> const & rowoffset,
	vector<uint> const & planeoffset,
	string const & description) :
		gfxdef(id, description),
		m_width(width),
		m_height(height),
		m_bitdepth(bitdepth),
		m_datasize(width * height * bitdepth),
		m_datasize_bytes(m_datasize / 8 + (m_datasize % 8 > 0 ? 1 : 0)),
		m_pixeloffsets(pixeloffset),
		m_rowoffsets(rowoffset),
		m_planeoffsets(planeoffset)
{
}

auto chrdef::width() const -> uint
{
	return m_width;
}

uint chrdef::height() const
{
	return m_height;
}

uint chrdef::bitdepth() const
{
	return m_bitdepth;
}

uint chrdef::datasize() const
{
	return m_datasize;
}

uint chrdef::datasize_bytes() const
{
	return m_datasize_bytes;
}

vector<uint> const & chrdef::plane_offsets() const
{
	return m_planeoffsets;
}

vector<uint> const & chrdef::pixel_offsets() const
{
	return m_pixeloffsets;
}

vector<uint> const & chrdef::row_offsets() const
{
	return m_rowoffsets;
}

uint chrdef::plane_offset_at(uint const index) const
{
	return m_planeoffsets[index];
}

uint chrdef::pixel_offset_at(uint const index) const
{
	return m_pixeloffsets[index];
}

uint chrdef::row_offset_at(uint const index) const
{
	return m_rowoffsets[index];
}

} // namespace chrgfx
