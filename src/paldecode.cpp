#include "paldecode.hpp"
#include <algorithm>

namespace chrgfx
{
palette pal_decode_calc(const pal_def* paldef, const u8* data)
{
	u8 temparr[paldef->entry_size];
	palette out = palette();
	for(u16 pal_iter = 0; pal_iter < paldef->palette_size;
			pal_iter += paldef->entry_size)
	{
		// TODO - elsewhere, determine endianness of local system
		// TODO - add endianness to color def (default big?)
		// TODO - check color endianness here - if it does not match system, do
		// reverse copy
		endianness sysorder = get_system_endianness();
		if(sysorder == paldef->colordef->byteorder)
		{
			std::copy(data + pal_iter, data + (pal_iter + paldef->entry_size),
								temparr);
		}
		else
		{
			std::reverse_copy(data + pal_iter, data + (pal_iter + paldef->entry_size),
												temparr);
		}
		u32* rawcolor = reinterpret_cast<u32*>(temparr);
		color t = get_color(paldef->colordef, *rawcolor);
		out.push_back(t);
	}
	return out;
};

palette pal_decode_fixed(const pal_def* paldef, const u8* data)
{
	return palette();
};

palette pal_decode_soft_tlp(const pal_def* paldef, const u8* data)
{
	return palette();
};

/*
pal_decode_calc::pal_decode_calc(pal_def_calc paldef)
{
	this->paldef = paldef;
};

palette pal_decode_calc::get_pal(u8* data)
{
	u8 temparr[this->paldef.entry_size];
	palette out = palette();
	for(u16 pal_iter = 0; pal_iter < this->paldef.syspal_size;
			pal_iter += this->paldef.entry_size)
	{
		std::copy(data + pal_iter, data + (pal_iter + this->paldef.entry_size),
							temparr);
		// todo: consider endianness!
		// std::reverse_copy(data + pal_iter,
		//										data + (pal_iter + this->paldef.entry_size), temparr);
		u32* rawcolor = reinterpret_cast<u32*>(temparr);
		color t = this->get_color(*rawcolor);
		out.push_back(t);
	}
	return out;
}
 */
color get_color(const color_def* colordef, const u32 data)
{
	/*
psuedo:
check if color_def is null

-ptr to u32 data
-for each pass
	-shift red, apply mask, OR with R var
	-shift green, apply mask, OR with G var
	-shift blue, apply mask, OR with B var
-expand R, G, B
TODO: how does MAME expand bits?

 */
	u8 red{0}, green{0}, blue{0};
	for(u16 pass_iter = 0; pass_iter < colordef->passes; pass_iter++)
	{
		red |= (data >> colordef->red_shift[pass_iter]) &
					 colordef->red_mask[pass_iter];
		green |= (data >> colordef->green_shift[pass_iter]) &
						 colordef->green_mask[pass_iter];
		blue |= (data >> colordef->blue_shift[pass_iter]) &
						colordef->blue_mask[pass_iter];
	}

	return color(red, green, blue);
};

}	// namespace chrgfx