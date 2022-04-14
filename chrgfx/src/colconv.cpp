#include "colconv.hpp"
#include "utils.hpp"

namespace chrgfx
{
using namespace png;

u32 encode_col(rgbcoldef const & rgbcoldef, color const & color)
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

	u32 out { 0 };
	u8 bitdepth = rgbcoldef.bitdepth();
	byte_t red { reduce_bitdepth(color.red, bitdepth) }, red_pass_shift { 0 },
			green { reduce_bitdepth(color.green, bitdepth) }, green_pass_shift { 0 },
			blue { reduce_bitdepth(color.blue, bitdepth) }, blue_pass_shift { 0 };

	u8 bitmask { 0 };
	u32 temp;

	for(auto const & this_pass : rgbcoldef.layout())
	{
		bitmask = (create_bitmask8(this_pass.red_size())) << red_pass_shift;
		temp = ((red & bitmask) >> red_pass_shift) << this_pass.red_shift();
		out |= temp;
		red_pass_shift += this_pass.red_size();

		bitmask = (create_bitmask8(this_pass.green_size())) << green_pass_shift;
		temp = ((green & bitmask) >> green_pass_shift) << this_pass.green_shift();
		out |= temp;
		green_pass_shift += this_pass.green_size();

		bitmask = (create_bitmask8(this_pass.blue_size())) << blue_pass_shift;
		temp = ((blue & bitmask) >> blue_pass_shift) << this_pass.blue_shift();
		out |= temp;
		blue_pass_shift += this_pass.blue_size();
	}

	return out;
}

u32 encode_col(refcoldef const & refcoldef, color const & color)
{
	return refcoldef.reftabIndex(color);
}

color decode_col(rgbcoldef const & rgbcoldef, u32 const color)
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
	u8 red { 0 }, green { 0 }, blue { 0 };
	u8 red_bitcount { 0 }, green_bitcount { 0 }, blue_bitcount { 0 };
	u8 bitmask { 0 };
	for(rgb_layout const & this_pass : rgbcoldef.layout())
	{
		bitmask = create_bitmask8(this_pass.red_size());
		red |= ((color >> this_pass.red_shift()) & bitmask) << red_bitcount;
		red_bitcount += this_pass.red_size();

		bitmask = create_bitmask8(this_pass.green_size());
		green |= ((color >> this_pass.green_shift()) & bitmask) << green_bitcount;
		green_bitcount += this_pass.green_size();

		bitmask = create_bitmask8(this_pass.blue_size());
		blue |= ((color >> this_pass.blue_shift()) & bitmask) << blue_bitcount;
		blue_bitcount += this_pass.blue_size();
	}

	red = expand_bitdepth(red, red_bitcount);
	green = expand_bitdepth(green, green_bitcount);
	blue = expand_bitdepth(blue, blue_bitcount);

	return png::color(red, green, blue);
}

color decode_col(refcoldef const & refcoldef, u32 const color)
{
	return refcoldef.reftabColor(color);
}

} // namespace chrgfx
