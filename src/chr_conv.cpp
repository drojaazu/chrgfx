#include "chr_conv.hpp"

#include <iomanip>

namespace chrgfx
{
	/*
		TILE CONVERSION ROUTINES
		Any custom tile conversion functions should take a reference to a chr_def and a pointer to the array of raw byte data. It will return an array of converted byte data.

		The converted byte data should be an array of unsigned values, each as one pixel referencing a value in the output palette.
	*/


	/**
	 * Returns raw tile data from a 
	 */
u8 const *get_chr(chr_def &chrdef, u8 const *data)
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

				work_pixel |= ((work_byte << work_bit) & 0x80) >>
											((8 - chrdef.get_bitplanes()) + plane_iter);
			}
			out_chr[this_pixel++] = work_pixel;
		}
	}

	return out_chr;
}

} // namespace chrgfx