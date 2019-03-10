#include "tlp_palette.hpp"

using namespace png;

namespace chrgfx
{
const pal_traits tilelayerpro_px::traits{255, 3};

const pal_traits* tilelayerpro_px::get_traits()
{
	return &tilelayerpro_px::traits;
}

const palette* tilelayerpro_px::get_pal_tlp(u8* data)
{
	if(data[0] != 0x54 || data[1] != 0x50 || data[2] != 0x4c)
		std::cerr << "Warning: Does not appear to be a valid TLP palette"
							<< std::endl;

	switch(data[3])
	{
		case 0:
		{
			// rgb
			data += 4;
			// return palette_converter::get_palette_generic(this, data, 3, 256);
			auto _out = new palette();
			_out->reserve(256);

			for(size_t palEntryIter = 0; palEntryIter < 256; palEntryIter++)
			{
				_out->push_back(color(data[0], data[1], data[2]));
				data += 3;
			}
			return _out;
			break;
		}
		case 1:
		{
			// famicom
			// throw std::exception("Type 1 TLP palettes currently unsupported");
			break;
		}
		case 2:
		{
			// sfc
			// throw std::exception("Type 2 TLP palettes currently unsupported");
			break;
		}
		default:
		{
			std::cerr << "Invalid TLP palette type value; processing as RGB"
								<< std::endl;
			break;
		}
	}
	return nullptr;
}

const color* tilelayerpro_px::get_rgb(u8* data)
{
	return new color(data[0], data[1], data[2]);
}

const palette* tilelayerpro_px::get_pal(u8* data) { return get_pal_tlp(data); }
}	// namespace chrgfx