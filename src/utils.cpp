#include "utils.hpp"

using namespace png;

namespace chrgfx
{
palette* make_pal(bool blank)
{
	auto outpal = new palette();
	outpal->reserve(256);

	if(blank)
	{
		outpal->insert(outpal->begin(), 256, color(0, 0, 0));
	}
	else
	{
		// basic 16 color palette based on Xterm colors
		// repeated 16x for 256 entry 8bpp palette
		for(uint8_t l = 0; l < 16; l++)
		{
			outpal->push_back(color(0, 0, 0));
			outpal->push_back(color(128, 0, 0));
			outpal->push_back(color(0, 128, 0));
			outpal->push_back(color(128, 128, 0));

			outpal->push_back(color(0, 0, 128));
			outpal->push_back(color(128, 0, 128));
			outpal->push_back(color(0, 128, 128));
			outpal->push_back(color(192, 192, 192));

			outpal->push_back(color(128, 128, 128));
			outpal->push_back(color(255, 0, 0));
			outpal->push_back(color(0, 255, 0));
			outpal->push_back(color(255, 255, 0));

			outpal->push_back(color(0, 0, 255));
			outpal->push_back(color(255, 0, 255));
			outpal->push_back(color(0, 255, 255));
			outpal->push_back(color(255, 255, 255));
		}
	}
	return outpal;
}

/*std::vector<const chr*>* get_bank(chr_xform* xform, u8* data, size_t count)
{
	if(count < 1) throw std::length_error("CHR count needs to be greater than 0");

	auto outbank = new std::vector<const chr*>();
	outbank->reserve(count);
	auto dataSize = xform->get_traits()->data_size;

	for(size_t countIter = 0; countIter < count; countIter++)
	{
		outbank->push_back(xform->get_chr(data));
		data += dataSize;
	}

	return outbank;
}*/

palette* get_pal(pal_xform* xform, u8* data, size_t count)
{
	auto outpal = new palette();
	outpal->reserve(256);
	auto datasize = xform->get_traits()->color_size;

	for(size_t paliter = 0; paliter < count; paliter++)
	{
		outpal->push_back(*(xform->get_rgb(data)));
		data += datasize;
	}

	// fill_pal(outpal);

	return outpal;
}

// fill in any blank entries in a palette to bring it up to 256
void fill_pal(palette* pal)
{
	if(pal->size() >= 256) return;

	u8 toFill = 256 - pal->size();

	for(u8 fillIter = 0; fillIter < toFill; fillIter++)
		pal->push_back(color(0, 0, 0));

	return;
}

}	// namespace chrgfx