#include "colconv.hpp"
#include "utils.hpp"

using namespace std;

namespace chrgfx
{

void encode_col(rgbcoldef const & rgbcoldef, rgb_color const * in_color, uint32 * out_color)
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

	// clang-format off
	uint8
		red {reduce_bitdepth(in_color->red, rgbcoldef.bitdepth())},
		green {reduce_bitdepth(in_color->green, rgbcoldef.bitdepth())},
		blue {reduce_bitdepth(in_color->blue, rgbcoldef.bitdepth())},
		red_bitcount {0}, green_bitcount {0}, blue_bitcount {0},
		bitmask;
	// clang-format on

	for (auto const & layout : rgbcoldef.layout())
	{
		bitmask = (create_bitmask8(layout.red_size())) << red_bitcount;
		*out_color |= ((red & bitmask) >> red_bitcount) << layout.red_offset();
		red_bitcount += layout.red_size();

		bitmask = (create_bitmask8(layout.green_size())) << green_bitcount;
		*out_color |= ((green & bitmask) >> green_bitcount) << layout.green_offset();
		green_bitcount += layout.green_size();

		bitmask = (create_bitmask8(layout.blue_size())) << blue_bitcount;
		*out_color |= ((blue & bitmask) >> blue_bitcount) << layout.blue_offset();
		blue_bitcount += layout.blue_size();
	}
}

void encode_col(refcoldef const & refcoldef, rgb_color const * in_color, uint32 * out_color)
{
	*out_color = refcoldef.by_color(*in_color);
}

void decode_col(rgbcoldef const & rgbcoldef, uint32 const * in_color, rgb_color * out_color)
{
	/*
pseudo:
-u32 data
-for each pass
	-shift color down by red offset, apply mask, shift up by bitcount, OR with current RED
	-shift color down by green offset, apply mask, shift up by bitcount, OR with current GREEN
	-shift color down by blue offset, apply mask, shift up by bitcount, OR with current BLUE
-expand bits for each channel
 */

	// clang-format off
	uint8
		red {0}, green {0}, blue {0},
		red_bitcount {0}, green_bitcount {0}, blue_bitcount {0},
		bitmask;
	// clang-format on

	for (rgb_layout const & this_pass : rgbcoldef.layout())
	{
		bitmask = create_bitmask8(this_pass.red_size());
		red |= (((*in_color >> this_pass.red_offset()) & bitmask) << red_bitcount);
		red_bitcount += this_pass.red_size();

		bitmask = create_bitmask8(this_pass.green_size());
		green |= (((*in_color >> this_pass.green_offset()) & bitmask) << green_bitcount);
		green_bitcount += this_pass.green_size();

		bitmask = create_bitmask8(this_pass.blue_size());
		blue |= (((*in_color >> this_pass.blue_offset()) & bitmask) << blue_bitcount);
		blue_bitcount += this_pass.blue_size();
	}

	red = expand_bitdepth(red, rgbcoldef.bitdepth());
	green = expand_bitdepth(green, rgbcoldef.bitdepth());
	blue = expand_bitdepth(blue, rgbcoldef.bitdepth());
	*out_color = rgb_color(red, green, blue);
}

void decode_col(refcoldef const & refcoldef, uint32 const * in_color, rgb_color * out_color)
{
	*out_color = refcoldef.by_value(*in_color);
}

} // namespace chrgfx
