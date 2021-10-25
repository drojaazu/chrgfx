#include "chrconv.hpp"

namespace chrgfx
{

byte_t * to_formatted_chr(chrdef const & chrdef, byte_t const * basic_chr)
{
	auto out_data = new u8[chrdef.datasize() / 8];
	to_formatted_chr_fast(chrdef, basic_chr, out_data);
	return out_data;
}

void to_formatted_chr_fast(chrdef const & chrdef, byte_t const * in_basic_chr,
													 byte_t * out_formatted_chr)
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
	ushort line_iter { 0 }, pixel_iter { 0 }, plane_iter { 0 }, curr_pixel { 0 },
			curr_bit { 0 }, bitpos_y { 0 }, bitpos_x { 0 }, bitpos { 0 },
			pixel_count { 0 };

	// auto out_data = new u8[chrdef.datasize() / 8];

	// for every line...
	for(line_iter = 0; line_iter < chrdef.height(); ++line_iter)
	{
		bitpos_y = chrdef.rowOffsetAt(line_iter);

		// for every pixel...
		for(pixel_iter = 0; pixel_iter < chrdef.width(); ++pixel_iter)
		{
			// TODO turn this into a pointer
			curr_pixel = in_basic_chr[pixel_count++];

			// if all the bit planes are unset (i.e. the value is zero)
			// we can skip the bitplane shenanigans altogether
			if(curr_pixel == 0)
				continue;

			bitpos_x = chrdef.pixelOffsetAt(pixel_iter);
			// for every bit plane...
			for(plane_iter = 0; plane_iter < chrdef.bitdepth(); ++plane_iter)
			{
				// extract the bit from the current bitplane
				curr_bit = (curr_pixel & (1 << plane_iter));
				// if the bit is unset, then do not set the equivalent bit in the
				// output
				if(curr_bit == 0)
					continue;

				// get the position in the output data for this bit
				bitpos = bitpos_y + bitpos_x + chrdef.planeOffsetAt(plane_iter);
				// out_data[bitpos / 8] |= (0x80 >> (bitpos % 8));
				*(out_formatted_chr + (bitpos >> 3)) |= (0x80 >> (bitpos % 8));
			}
		}
	}

	// return out_data;
}

byte_t * to_basic_chr(chrdef const & chrdef, byte_t const * formatted_chr)
{
	auto out_data = new u8[chrdef.datasize() / 8];
	to_basic_chr_fast(chrdef, formatted_chr, out_data);
	return out_data;
}

void to_basic_chr_fast(chrdef const & chrdef, byte_t const * in_formatted_chr,
											 byte_t * out_basic_chr)
{
	ushort line_iter { 0 }, pixel_iter { 0 }, plane_iter { 0 }, curr_pixel,
			work_byte_t { 0 }, work_bit { 0 }, bitpos_y { 0 }, bitpos_x { 0 },
			bitpos { 0 }, this_pixel { 0 };

	// auto out_data = new byte[chrdef.datasize() / 8];

	/*
	#ifdef DEBUG
		std::cerr << "This chr data:" << std::endl;
		std::cerr << std::hex;
		for(s16 datadump = 0; datadump < (chrdef->charincrement / 8);
	++datadump) { std::cerr << std::setw(2) << std::setfill('0') <<
	(int)data[datadump]
								<< " ";
		}
		std::cerr << std::dec << std::endl << std::endl;
	#endif
	*/

	// for every line...
	for(line_iter = 0; line_iter < chrdef.height(); ++line_iter)
	{
		bitpos_y = chrdef.rowOffsetAt(line_iter);

		// for every pixel in the line...
		for(pixel_iter = 0; pixel_iter < chrdef.width(); ++pixel_iter)
		{
			bitpos_x = chrdef.pixelOffsetAt(pixel_iter);
			curr_pixel = 0;

			// for every bit planee
			for(plane_iter = 0; plane_iter < chrdef.bitdepth(); ++plane_iter)
			{
				bitpos = bitpos_y + bitpos_x + chrdef.planeOffsetAt(plane_iter);

				work_byte_t = in_formatted_chr[bitpos / 8];
				// if work_byte_t is 0, no bits are set, so no bits will be set in the
				// output, so let's move to the next byte
				if(work_byte_t == 0)
					continue;
				work_bit = bitpos % 8;

				// curr_pixel |= ((work_byte_t << curr_bit) & 0x80) >>
				//							((8 - chrdef.get_bitplanes()) + plane_iter);
				// curr_pixel |= (work_byte_t & (0x80 >> curr_bit)) >> (7 -
				// plane_iter);
				curr_pixel |= ((work_byte_t << work_bit) & 0x80) >> (7 - plane_iter);
			}
			// TODO turn this into a pointer
			// out_data[this_pixel++] = curr_pixel;
			*(out_basic_chr + this_pixel) = curr_pixel;
			++this_pixel;
		}
	}
}

} // namespace chrgfx
