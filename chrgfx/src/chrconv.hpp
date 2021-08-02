#ifndef CHRGFX__CONV_CHR_H
#define CHRGFX__CONV_CHR_H

#include "buffer.hpp"
#include "chrdef.hpp"
#include "types.hpp"
//#include <iomanip>
#include <map>
#include <string>

namespace chrgfx
{
	namespace converters
	{
		/**
		 * Represents a function to convert a tile to a given encoding
		 */
		// using ChrConverterT = buffer (*)(chrdef const &, buffer const &);

		/**
		 * Convert a tile to the specified encoding
		 */
		void toFormattedChr(chrdef const & to_def, u8 const * in_data,
												u8 * out_data)
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
			s16 line_iter { 0 }, pixel_iter { 0 }, plane_iter { 0 };
			u16 curr_pixel { 0 }, curr_bit { 0 }, bitpos_y { 0 }, bitpos_x { 0 },
					bitpos { 0 }, pixel_count { 0 };

			// auto out = buffer(to_def.getDataSize() / 8, 0);

			// for every line...
			for(line_iter = 0; line_iter < to_def.getHeight(); ++line_iter)
			{
				bitpos_y = to_def.getRowOffsetAt(line_iter);

				// for every pixel...
				for(pixel_iter = 0; pixel_iter < to_def.getWidth(); ++pixel_iter)
				{
					curr_pixel = in_data[pixel_count++];

					// if all the bit planes are unset (i.e. the value is zero)
					// we can skip the bitplane shenanigans altogether
					if(curr_pixel == 0)
						continue;

					bitpos_x = to_def.getPixelOffsetAt(pixel_iter);
					// for every bit plane...
					for(plane_iter = 0; plane_iter < to_def.getBitdepth(); ++plane_iter)
					{
						// extract the bit from the current bitplane
						curr_bit = (curr_pixel & (1 << plane_iter));
						// if the bit is unset, then do not set the equivalent bit in the
						// output
						if(curr_bit == 0)
							continue;

						// get the position in the output data for this bit
						bitpos = bitpos_y + bitpos_x + to_def.getPlaneOffsetAt(plane_iter);
						out_data[bitpos / 8] |= (0x80 >> (bitpos % 8));
					}
				}
			}

			// return out;
		}

		/**
		 * Convert a tile from the specified encoding
		 */
		void toBasicChr(chrdef const & from_def, u8 const * in_data, u8 * out_data)
		{
			s16 line_iter { 0 }, pixel_iter { 0 }, plane_iter { 0 };
			u16 curr_pixel, work_byte { 0 }, work_bit { 0 }, bitpos_y { 0 },
					bitpos_x { 0 }, bitpos { 0 }, this_pixel { 0 };

			// auto out = buffer(from_def.getWidth() * from_def.getHeight(), 0);

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
			for(line_iter = 0; line_iter < from_def.getHeight(); ++line_iter)
			{
				bitpos_y = from_def.getRowOffsetAt(line_iter);

				// for every pixel in the line...
				for(pixel_iter = 0; pixel_iter < from_def.getWidth(); ++pixel_iter)
				{
					bitpos_x = from_def.getPixelOffsetAt(pixel_iter);
					curr_pixel = 0;

					// for every bit planee
					for(plane_iter = 0; plane_iter < from_def.getBitdepth(); ++plane_iter)
					{
						bitpos =
								bitpos_y + bitpos_x + from_def.getPlaneOffsetAt(plane_iter);

						work_byte = in_data[bitpos / 8];
						// if work_byte is 0, no bits are set, so no bits will be set in the
						// output, so let's move to the next byte
						if(work_byte == 0)
							continue;
						work_bit = bitpos % 8;

						// curr_pixel |= ((work_byte << curr_bit) & 0x80) >>
						//							((8 - chrdef.get_bitplanes()) + plane_iter);
						// curr_pixel |= (work_byte & (0x80 >> curr_bit)) >> (7 -
						// plane_iter);
						curr_pixel |= ((work_byte << work_bit) & 0x80) >> (7 - plane_iter);
					}
					out_data[this_pixel++] = curr_pixel;
				}
			}

			// return out;
		}

	} // namespace converters
} // namespace chrgfx

#endif