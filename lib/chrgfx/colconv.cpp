#include "colconv.hpp"
#include "utils.hpp"

using namespace std;

namespace chrgfx
{

void encode_col(rgbcoldef const * rgbcoldef, rgb_color const * in_color, uint32 * out_color)
{
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

	uint8 bitdepth = rgbcoldef->bitdepth();
	uint8 red {reduce_bitdepth(in_color->red, bitdepth)}, red_pass_shift {0},
		green {reduce_bitdepth(in_color->green, bitdepth)}, green_pass_shift {0},
		blue {reduce_bitdepth(in_color->blue, bitdepth)}, blue_pass_shift {0};

	uint8 bitmask;
	uint32 temp;

	for (auto const & this_pass : rgbcoldef->layout())
	{
		bitmask = (create_bitmask8(this_pass.red_size())) << red_pass_shift;
		temp = ((red & bitmask) >> red_pass_shift) << this_pass.red_shift();
		*out_color |= temp;
		red_pass_shift += this_pass.red_size();

		bitmask = (create_bitmask8(this_pass.green_size())) << green_pass_shift;
		temp = ((green & bitmask) >> green_pass_shift) << this_pass.green_shift();
		*out_color |= temp;
		green_pass_shift += this_pass.green_size();

		bitmask = (create_bitmask8(this_pass.blue_size())) << blue_pass_shift;
		temp = ((blue & bitmask) >> blue_pass_shift) << this_pass.blue_shift();
		*out_color |= temp;
		blue_pass_shift += this_pass.blue_size();
	}
}

void encode_col(refcoldef const * refcoldef, rgb_color const * in_color, uint32 * out_color)
{
	*out_color = refcoldef->by_color(*in_color);
}

void decode_col(rgbcoldef const * rgbcoldef, uint32 const * in_color, rgb_color * out_color)
{

	/*
psuedo:
-u32 data
-for each pass
	-shift red, apply mask, OR with R var
	-shift green, apply mask, OR with G var
	-shift blue, apply mask, OR with B var
-expand R, G, B

 */
	uint8 red {0}, green {0}, blue {0};
	uint8 red_bitcount {0}, green_bitcount {0}, blue_bitcount {0};
	uint8 bitmask;
	for (rgb_layout const & this_pass : rgbcoldef->layout())
	{
		bitmask = create_bitmask8(this_pass.red_size());
		red |= ((*in_color >> this_pass.red_shift()) & bitmask) << red_bitcount;
		red_bitcount += this_pass.red_size();

		bitmask = create_bitmask8(this_pass.green_size());
		green |= ((*in_color >> this_pass.green_shift()) & bitmask) << green_bitcount;
		green_bitcount += this_pass.green_size();

		bitmask = create_bitmask8(this_pass.blue_size());
		blue |= ((*in_color >> this_pass.blue_shift()) & bitmask) << blue_bitcount;
		blue_bitcount += this_pass.blue_size();
	}

	red = expand_bitdepth(red, red_bitcount);
	green = expand_bitdepth(green, green_bitcount);
	blue = expand_bitdepth(blue, blue_bitcount);
	*out_color = rgb_color(red, green, blue);
}

void decode_col(refcoldef const * refcoldef, uint32 const * in_color, rgb_color * out_color)
{
	*out_color = refcoldef->by_value(*in_color);
}

} // namespace chrgfx
