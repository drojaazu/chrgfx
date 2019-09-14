#include "chrdecode.hpp"

namespace chrgfx
{
const u8* get_chr(const chr_def* chrdef, const u8* data)
{
	u8 work_pixel, this_pixel{0}, work_byte{0}, work_bit{0};
	s16 line_iter{0}, pixel_iter{0}, plane_iter{0};
	u16 line_offset, pixel_offset, plane_offset;

	u8* out_chr = new u8[chrdef->width * chrdef->height];

	// for every line...
	for(line_iter = 0; line_iter < chrdef->height; ++line_iter)
	{
		line_offset = chrdef->yoffset[line_iter];

		// for every pixel in the line...
		for(pixel_iter = 0; pixel_iter < chrdef->width; ++pixel_iter)
		{
			pixel_offset = line_offset + chrdef->xoffset[pixel_iter];
			work_pixel = 0;

			// for every bit plane in the pixel...
			for(plane_iter = 0; plane_iter < chrdef->planes; ++plane_iter)
			{
				plane_offset = pixel_offset + chrdef->planeoffset[plane_iter];

				work_byte = data[plane_offset / 8];
				work_bit = plane_offset % 8;

				work_pixel |= ((work_byte << work_bit) & 0x80) >>
											((8 - chrdef->planes) + plane_iter);
			}
			out_chr[this_pixel++] = work_pixel;
		}
	}

	return out_chr;
}

}	// namespace chrgfx