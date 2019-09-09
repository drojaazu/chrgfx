#include "chrdecode.hpp"

namespace chrgfx
{
chr_decode::chr_decode(const chr_def chrdef) { this->chrdef = chrdef; }

const chr_def* chr_decode::get_def() { return &this->chrdef; }

const u8* chr_decode::get_chr(const u8* data)
{
	u8 work_pixel, this_pixel{0}, work_byte{0}, work_bit{0};
	s16 line_iter{0}, pixel_iter{0}, plane_iter{0};
	u32 this_bit{0};
	u8* out_chr = new u8[this->chrdef.width * this->chrdef.height];

	u16 line_offset, pixel_offset, plane_offset;

	// for every line...
	for(line_iter = 0; line_iter < this->chrdef.height; line_iter++)
	{
		line_offset = this->chrdef.yoffset[line_iter];

		// for every pixel in the line...
		for(pixel_iter = 0; pixel_iter < this->chrdef.width; pixel_iter++)
		{
			pixel_offset = line_offset + this->chrdef.xoffset[pixel_iter];
			work_pixel = 0;

			// for every bit plane in the pixel...
			for(plane_iter = 0; plane_iter < this->chrdef.planes; plane_iter++)
			{
				plane_offset = pixel_offset + this->chrdef.planeoffset[plane_iter];

				work_byte = data[plane_offset / 8];
				work_bit = plane_offset % 8;

				work_pixel |= ((work_byte << work_bit) & 0x80) >>
											((8 - this->chrdef.planes) + plane_iter);
			}
			// out_chr[--this_pixel] = work_pixel;
			out_chr[this_pixel++] = work_pixel;
		}
	}

	return out_chr;
}

}	// namespace chrgfx