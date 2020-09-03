#include "conv_col.hpp"
#include "gfxdef.hpp"

namespace chrgfx
{
namespace conv_color
{

// converter maps
std::map<string const, cvto_col_t> const converters_to{{"default", cvto_color}};

std::map<string const, cvfrom_col_t> const converters_from{
		{"default", cvfrom_color}};

png::color cvfrom_color(coldef const &from_coldef, u32 const data)
{
	if(from_coldef.use_reftab()) {
		return from_coldef.get_reftab_entry(data);
	} else {

		/*
	psuedo:
	-u32 data
	-for each pass
		-shift red, apply mask, OR with R var
		-shift green, apply mask, OR with G var
		-shift blue, apply mask, OR with B var
	-expand R, G, B

	 */
		u8 red{0}, green{0}, blue{0};
		u8 red_bitcount{0}, green_bitcount{0}, blue_bitcount{0};
		u8 bitmask{0};
		for(rgb_layout const &this_pass : from_coldef.get_rgb_layout()) {
			bitmask = create_bitmask8(this_pass.get_red_size());
			red |= ((data >> this_pass.get_red_shift()) & bitmask) << red_bitcount;
			red_bitcount += this_pass.get_red_size();

			bitmask = create_bitmask8(this_pass.get_green_size());
			green |= ((data >> this_pass.get_green_shift()) & bitmask)
							 << green_bitcount;
			green_bitcount += this_pass.get_green_size();

			bitmask = create_bitmask8(this_pass.get_blue_size());
			blue |= ((data >> this_pass.get_blue_shift()) & bitmask) << blue_bitcount;
			blue_bitcount += this_pass.get_blue_size();
		}

		red = expand_bits(red, red_bitcount);
		green = expand_bits(green, green_bitcount);
		blue = expand_bits(blue, blue_bitcount);

		return color(red, green, blue);
	}
};

u32 cvto_color(coldef const &to_coldef, png::color const data)
{
	if(to_coldef.use_reftab()) {
		return to_coldef.get_reftab_idx(data);
	} else {
		/*
			seperate r g b from color

			for each channel, reduce to the number of bits available

			track shift for individual color (increases by size each pass)
			for each pass:
			 red:
				- create bitmask based on size
				- shift bitmask
				- AND bitmask to red
				- shift masked color back to 0
				- shift left as defined, OR on to out value
			green:
			blue:
			 as above
		*/

		u32 out{0};
		u8 bitdepth = to_coldef.get_bitdepth();
		u8 red{reduce_bits(data.red, bitdepth)}, red_pass_shift{0},
				green{reduce_bits(data.green, bitdepth)}, green_pass_shift{0},
				blue{reduce_bits(data.blue, bitdepth)}, blue_pass_shift{0};

		u8 bitmask{0};
		u32 temp;

		for(rgb_layout const &this_pass : to_coldef.get_rgb_layout()) {
			bitmask = (create_bitmask8(this_pass.get_red_size())) << red_pass_shift;
			temp = ((red & bitmask) >> red_pass_shift) << this_pass.get_red_shift();
			out |= temp;
			red_pass_shift += this_pass.get_red_size();

			bitmask = (create_bitmask8(this_pass.get_green_size()))
								<< green_pass_shift;
			temp = ((green & bitmask) >> green_pass_shift)
						 << this_pass.get_green_shift();
			out |= temp;
			green_pass_shift += this_pass.get_green_size();

			bitmask = (create_bitmask8(this_pass.get_blue_size())) << blue_pass_shift;
			temp = ((blue & bitmask) >> blue_pass_shift)
						 << this_pass.get_blue_shift();
			out |= temp;
			blue_pass_shift += this_pass.get_blue_size();
		}

		return out;
	}
}

} // namespace conv_color
} // namespace chrgfx