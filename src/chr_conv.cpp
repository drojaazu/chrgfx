#include "chr_conv.hpp"

#include <iomanip>

namespace chrgfx
{
/*
	TILE CONVERSION ROUTINES
	Any custom tile conversion functions should take a reference to a chr_def and
	a pointer to the array of raw byte data. It will return an array of converted
	byte data.

	The converted byte data should be an array of unsigned values, each as one
	pixel referencing a value in the output palette.
*/

/**
 * Returns a CHR from given raw tile bitmap
 */
bptr to_chr(chr_def &chrdef, bptr data)
{

	/*
		-for every line...
		--for every pixel...
		---work pixel
		bit position in output data: yoffset[curline] + xoffset[curpixel] +
		poffset[curr]
		--- cache yoffset[curline] + xoffset[curpixel] value
		---for every bit plane...
		---extract bit from input data
		---if bit is zero, no need to do anything
		---output data |= (input_bit << bit position mod)
	*/
	u8 work_pixel, work_bit, workbyte, workbit;
	s16 line_iter{0}, pixel_iter{0}, plane_iter{0};
	u16 bitpos_y{0}, bitpos_x{0}, bitpos_p{0}, bitpos{0};
	u8 *out = new u8[chrdef.get_datasize() / 8]{0};

	// for every line...
	for(line_iter = 0; line_iter < chrdef.get_height(); ++line_iter) {
		bitpos_y = chrdef.get_yoffset_at(line_iter);
		// for every pixel...
		for(pixel_iter = 0; pixel_iter < chrdef.get_width(); ++pixel_iter) {
			bitpos_x = chrdef.get_xoffset_at(pixel_iter);
			work_pixel = data[(line_iter * chrdef.get_width()) + pixel_iter];
			// for every bit plane...

			for(plane_iter = 0; plane_iter < chrdef.get_bitplanes(); ++plane_iter) {
				work_bit = (work_pixel & (1 << plane_iter));
				// if the value is zero, we don't need to bother setting a bit on our
				// output data (since the data is already init'd to 0)
				if(work_bit == 0)
					continue;
				// get the position in the output data for this bit
				bitpos = bitpos_x + bitpos_y + chrdef.get_planeoffset_at(plane_iter);
				workbyte = (bitpos / 8);
				workbit = bitpos % 8;
				// warning: check for bounds here!
				out[workbyte] |= 0x80 >> workbit;
			}
		}
	}

	return out;
}

/**
 * Returns raw tile bitmap (packed) from a CHR
 */
bptr get_chr(chr_def &chrdef, bptr data)
{
	u8 work_pixel, this_pixel{0}, work_byte{0}, work_bit{0};
	s16 line_iter{0}, pixel_iter{0}, plane_iter{0};
	u16 line_offset, pixel_offset, plane_offset;

	u8 *out_chr = new u8[chrdef.get_width() * chrdef.get_height()];

	/*
	#ifdef DEBUG
		std::cerr << "This chr data:" << std::endl;
		std::cerr << std::hex;
		for(s16 datadump = 0; datadump < (chrdef->charincrement / 8); ++datadump) {
			std::cerr << std::setw(2) << std::setfill('0') << (int)data[datadump]
								<< " ";
		}
		std::cerr << std::dec << std::endl << std::endl;
	#endif
	*/

	// for every line...
	for(line_iter = 0; line_iter < chrdef.get_height(); ++line_iter) {
		line_offset = chrdef.get_yoffset()[line_iter];

		// for every pixel in the line...
		for(pixel_iter = 0; pixel_iter < chrdef.get_width(); ++pixel_iter) {
			pixel_offset = line_offset + chrdef.get_xoffset()[pixel_iter];
			work_pixel = 0;

			// for every bit plane in the pixel...
			for(plane_iter = 0; plane_iter < chrdef.get_bitplanes(); ++plane_iter) {
				plane_offset = pixel_offset + chrdef.get_planeoffset()[plane_iter];

				work_byte = data[plane_offset / 8];
				work_bit = plane_offset % 8;

				// work_pixel |= ((work_byte << work_bit) & 0x80) >>
				//							((8 - chrdef.get_bitplanes()) + plane_iter);

				work_pixel |= ((work_byte << work_bit) & 0x80) >> (7 - plane_iter);
			}
			out_chr[this_pixel++] = work_pixel;
		}
	}

	return out_chr;
}

} // namespace chrgfx