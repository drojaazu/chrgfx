#include "custom.hpp"
#include "builtin_defs.hpp"
#include "chrconv.hpp"
#include <stdexcept>

using namespace std;

namespace chrgfx::custom
{
void decode_chr_nintendo_sfc_3bpp(byte_t const * encoded_chr, size_t datasize, pixel * out)
{
	if (datasize != 64)
		throw runtime_error("invalid allocation for Super Famicom 3bpp tile (need 64 bytes)");

	// decode to 2bpp first, then apply the third bit plane
	// chrdef chrdef_2bpp {"", 8, 8, 2, {0, 8}, {0, 1, 2, 3, 4, 5, 6, 7}, {0, 16, 32, 48, 64, 80, 96, 112}};
	decode_chr(gfxdefs::chr_8x8_2bpp_packed_msb, encoded_chr, out);

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
}

void decode_pal_tilelayerpro(byte_t const * in_pal, size_t datasize, palette * out_pal)
{
	if (datasize < 6)
		throw runtime_error("not enough data for a valid TileLayer Pro palette");

	if (in_pal[0] != 'T' || in_pal[1] != 'P' || in_pal[2] != 'L')
		throw runtime_error("not a valid TileLayer Pro palette (invalid header)");

	if (in_pal[4] != 0)
		throw runtime_error("Unsupported TileLayer Pro palette format (only RGB mode supported)");

	size_t pal_index {0};
	for (size_t i {5}; i < (datasize - 5); i += 3)
		(*out_pal)[pal_index++] = rgb_color(in_pal[i], in_pal[i + 1], in_pal[i + 2]);
};

size_t encode_pal_tilelayerpro(palette const * palette, byte_t * out_pal)
{
	// 3 byte header, 1 byte type, 256 RGB entries
	size_t out_size = 3 + 1 + (256 * 3);
	auto out = (byte_t *) malloc(out_size);
	out_pal[0] = 'T';
	out_pal[1] = 'P';
	out_pal[2] = 'L';
	out_pal[3] = '0';

	size_t index {4};
	for (auto const & col : *palette)
	{
		out_pal[index++] = col.red;
		out_pal[index++] = col.green;
		out_pal[index++] = col.blue;
	}
	return out_size;
}

/*
void decode_pal_paintshoppro(byte_t const * in_pal, size_t datasize, basic_palette * out_pal)
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

size_t encode_pal_paintshoppro(basic_palette const * palette, byte_t * out_pal)
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
