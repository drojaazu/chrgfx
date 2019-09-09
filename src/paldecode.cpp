#include "paldecode.hpp"
#include <algorithm>

namespace chrgfx
{
pal_decode::pal_decode(pal_def paldef)
{
	this->paldef = paldef;
	// todo: make sure paldef has a non null sys pal, otherwise exception
	if(this->paldef.colordef == nullptr)
		this->use_syspal = true;
	else
		this->use_syspal = false;
}

const palette* const pal_decode::get_pal(const u8* data)
{
	/*

	 */
	u8 temparr[this->paldef.entry_size];
	palette* out = new palette();
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
		out->push_back(t);
	}
	return out;
}

const color pal_decode::get_color(const u32 data)
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
	if(this->use_syspal)
	{
		// if data > max_entries, error
		return this->paldef.syspal[data];
	}
	u8 red{0}, green{0}, blue{0};
	for(u16 pass_iter = 0; pass_iter < this->paldef.colordef->passes; pass_iter++)
	{
		red |= (data >> this->paldef.colordef->red_shift[pass_iter]) &
					 this->paldef.colordef->red_mask[pass_iter];
		green |= (data >> this->paldef.colordef->green_shift[pass_iter]) &
						 this->paldef.colordef->green_mask[pass_iter];
		blue |= (data >> this->paldef.colordef->blue_shift[pass_iter]) &
						this->paldef.colordef->blue_mask[pass_iter];
	}

	return color(red, green, blue);
}
}	// namespace chrgfx