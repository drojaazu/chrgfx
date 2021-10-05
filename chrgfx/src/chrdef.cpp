#include "chrdef.hpp"

namespace chrgfx
{
	using namespace std;

	ushort chrdef::width() const
	{
		return p_width;
	}

	ushort chrdef::height() const
	{
		return p_height;
	}

	ushort chrdef::bitdepth() const
	{
		return p_bitdepth;
	}

	ushort chrdef::datasize() const
	{
		return p_datasize;
	};

	ushort chrdef::planeOffsetAt(ushort index) const
	{
		return p_planeoffset[index];
	};

	ushort chrdef::pixelOffsetAt(ushort index) const
	{
		return p_pixeloffset[index];
	};

	ushort chrdef::rowOffsetAt(ushort index) const
	{
		return p_rowoffset[index];
	};

} // namespace chrgfx
