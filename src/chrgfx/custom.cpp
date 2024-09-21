#include "custom.hpp"
#include "chrconv.hpp"
#include "chrdef.hpp"

using namespace std;

namespace chrgfx::custom
{
byte_t * decode_chr_nintendo_sfc_3bpp(byte_t const * encoded_chr, byte_t * out)
{
	if (out == nullptr)
		out = new byte_t[8 * 8];

	// decode to 2bpp first, then apply the third bit plane
	chrdef chrdef_2bpp {"", 8, 8, 2, {0, 8}, {0, 1, 2, 3, 4, 5, 6, 7}, {0, 16, 32, 48, 64, 80, 96, 112}};
	decode_chr(chrdef_2bpp, encoded_chr, out);

	// the last 8 bytes of the data are the third bitplane values for each row
	byte_t const * ptr_bitplane3_data = encoded_chr + 16;
	byte_t * ptr_out_pixel = out;
	// for each row (each byte of input)
	for (auto i_row = 0; i_row < 8; ++i_row)
	{
		byte_t bitplane3 = *ptr_bitplane3_data++;
		// for each pixel
		for (auto i_pixel = 0; i_pixel < 8; ++i_pixel)
		{
			*ptr_out_pixel |= ((bitplane3 & byte_t(0x80)) >> 5);
			bitplane3 <<= 1;
		}
	}

	return out;
}

/*
palette decode_pal_tilelayerpro(istream & tpl_palette)
{
	if (! tpl_palette.good())
		throw runtime_error("Palette input stream is not in a good state");

	byte buffer[3];
	tpl_palette.read((char *) buffer, 3);
	if (tpl_palette.eof())
		throw invalid_argument("Reached EOF before reading file header");

	if (buffer[0] != byte('T') || buffer[1] != byte('P') || buffer[2] != byte('L'))
		throw invalid_argument("Not a TileLayer Pro palette (invalid header)");

	if (tpl_palette.get() != 0)
		throw std::invalid_argument("Unsupported TileLayer Pro palette format (only RGB mode supported)");

	blob<uint8_t[3]> data {tpl_palette};
	palette out;
	size_t c = 0;
	for (auto y : data)
	{
		// tpl_palette.read((char *) buffer, 3);
		// uint8_t * y = *t++;
		out[c] = color {y[0], y[1], y[2]};
	}
	return out;
};

void encode_pal_tilelayerpro(palette const & palette, ostream & output)
{
	if (! output.good())
		throw runtime_error("Output palette is not in a good state");

	output << "TPL";
	output << (char) 0;
	for (auto const & col : palette)
	{
		output << col.red << col.green << col.blue;
	}
	output.flush();
}

palette decode_pal_paintshoppro(istream & psp_palette)
{
	// psppalette is a text file (likely with windows newlines)
	// first line is "header" JASC-PAL
	// next is version? 0100
	// then number of entries in pal 256
	// then a list of space seperated unsigned chars, one for each channel (likely
	// R G B)

	if (! psp_palette.good())
		throw runtime_error("Palette input stream is not in a good state");

	string line;

	if (! getline(psp_palette, line))
		throw invalid_argument("Could not read file header");
	strip_cr(line);
	if (line != "JASC-PAL")
		throw invalid_argument("Invalid PaintShop Pro palette (invalid header)");

	if (! getline(psp_palette, line))
		throw invalid_argument("Could not read file version");
	strip_cr(line);
	if (line != "0100")
		throw invalid_argument("Invalid PaintShop Pro palette (unsupported version)");

	if (! getline(psp_palette, line))
		throw invalid_argument("Could not read entry count");
	strip_cr(line);
	// TODO catches around this
	size_t pal_size = stoul(line);

	palette out;
	out.reserve(pal_size);
	for (auto i = 0; i < pal_size; ++i)
	{
		if (! getline(psp_palette, line))
			throw invalid_argument("Could not read palette entry");
		strip_cr(line);

		auto rgb = sto_container<vector<uint8>>(line, ' ');
		out.emplace_back(rgb[0], rgb[1], rgb[2]);
	}

	return out;
}

void encode_pal_paintshoppro(palette const & palette, ostream & output)
{
	if (! output.good())
		throw runtime_error("Output palette stream is not in a usable state");
	output << "JASC-PAL\n";
	output << "010\n";
	output << palette.size() << '\n';
	for (auto const & color : palette)
		output << color.red << ' ' << color.green << ' ' << color.blue << '\n';
	output.flush();
}
*/

} // namespace chrgfx::custom
