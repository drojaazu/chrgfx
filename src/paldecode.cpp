#include "paldecode.hpp"
#include <algorithm>
#include "utils.hpp"

namespace chrgfx
{
/*
palette pal_decode_calc(const pal_def* paldef, const u8* data)
{
u16 bytestocopy =
		(paldef->entry_size / 8) + ((paldef->entry_size % 8) > 0 ? 1 : 0);
if(bytestocopy > 4)
{
	throw std::out_of_range("Color value can be no more than 4 bytes");
}
u32 bitmask = create_bitmask(paldef->entry_size);
u8 temparr[bytestocopy];

palette out = palette();
for(u16 pal_iter = 0; pal_iter < paldef->palette_size;
		pal_iter += bytestocopy)
{
	// TODO - elsewhere, determine endianness of local system
	// TODO - add endianness to color def (default big?)
	// TODO - check color endianness here - if it does not match system, do
	// reverse copy
	endianness sysorder = get_system_endianness();
	if(sysorder == paldef->colordef->byteorder)
	{
		std::copy(data + pal_iter, data + (pal_iter + bytestocopy), temparr);
	}
	else
	{
		std::reverse_copy(data + pal_iter, data + (pal_iter + bytestocopy),
											temparr);
	}
	u32 rawcolor = *reinterpret_cast<u32*>(temparr);
	rawcolor &= bitmask;
	color t = get_color(paldef->colordef, rawcolor);
	out.push_back(t);
}
return out;
};
*/
palette pal_decode_fixed(const pal_def* paldef, const u8* data)
{
	/*
	psuedo:
	 Loop on number of bytes for a full palette
		data size of palette = (bits/entry * entries/subpal * subpals/fullpal) / 8
		bits per entry = entry size % 8
		bytes per entry = (entry size / 8) + (bits per entry > 0 ? 1 : 0)

		for(data size of palette)
			if(bytes per entry == 1 && bits > 0)
				for(8 / bits per entry)
					data >> (iter * bits per entry)
					apply bitmask
					push value to palette
			else
				read 'bytes per entry' of data into temp array (reverse if endianness
	does not match)
				convert array to u32
				apply bitmask
				push value to palette

	 */
	// in bytes
	u16 pal_datasize =
			(paldef->entry_datasize * paldef->subpal_length * paldef->subpal_count) /
			8;
	u8 entry_bits = paldef->entry_datasize % 8;
	u8 entry_bytes = (paldef->entry_datasize / 8) + (entry_bits > 0 ? 1 : 0);

	u32 bitmask = create_bitmask(paldef->entry_datasize);

	palette out = palette();

	for(u16 data_iter{0}; data_iter < pal_datasize; data_iter += entry_bytes)
	{
		if(entry_bytes == 1 && entry_bits > 0)
		{
			u32 dataval{data[data_iter]};
			for(u8 shift_iter{0}; shift_iter < (8 / entry_bits); ++shift_iter)
			{
				out.push_back(paldef->syspal->at(
						(dataval >> (entry_bits * shift_iter)) & bitmask));
			}
		}
		else
		{
			u8 temparr[entry_bytes];
			endianness sysorder = get_system_endianness();
			if(sysorder == paldef->colordef->byteorder)
			{
				std::copy(data + data_iter, data + (data_iter + entry_bytes), temparr);
			}
			else
			{
				std::reverse_copy(data + data_iter, data + (data_iter + entry_bytes),
													temparr);
			}

			u32 rawcolor = *reinterpret_cast<u32*>(temparr);
			rawcolor &= bitmask;
			color t = paldef->syspal->at(rawcolor);
			out.push_back(t);
		}
	}
	return out;
};

palette pal_decode_calc(const pal_def* paldef, const u8* data)
{
	/*
	psuedo:
	 Loop on number of bytes for a full palette
		data size of palette = (bits/entry * entries/subpal * subpals/fullpal) / 8
		bits per entry = entry size % 8
		bytes per entry = (entry size / 8) + (bits per entry > 0 ? 1 : 0)

		for(data size of palette)
			if(bytes per entry == 1 && bits > 0)
				for(8 / bits per entry)
					data >> (iter * bits per entry)
					apply bitmask
					push value to palette
			else
				read 'bytes per entry' of data into temp array (reverse if endianness
	does not match)
				convert array to u32
				apply bitmask
				push value to palette

	 */
	// in bytes
	u16 pal_datasize =
			(paldef->entry_datasize * paldef->subpal_length * paldef->subpal_count) /
			8;
	u8 entry_bits = paldef->entry_datasize % 8;
	u8 entry_bytes = (paldef->entry_datasize / 8) + (entry_bits > 0 ? 1 : 0);

	u32 bitmask = create_bitmask(paldef->entry_datasize);

	palette out = palette();

	for(u16 data_iter{0}; data_iter < pal_datasize; data_iter += entry_bytes)
	{
		if(entry_bytes == 1 && entry_bits > 0)
		{
			u32 dataval{data[data_iter]};
			for(u8 shift_iter{0}; shift_iter < (8 / entry_bits); ++shift_iter)
			{
				color t = get_color(paldef->colordef,
														((dataval >> (entry_bits * shift_iter)) & bitmask));
			}
		}
		else
		{
			u8 temparr[entry_bytes];
			endianness sysorder = get_system_endianness();
			if(sysorder == paldef->colordef->byteorder)
			{
				std::copy(data + data_iter, data + (data_iter + entry_bytes), temparr);
			}
			else
			{
				std::reverse_copy(data + data_iter, data + (data_iter + entry_bytes),
													temparr);
			}

			u32 rawcolor = *reinterpret_cast<u32*>(temparr);
			rawcolor &= bitmask;
			color t = get_color(paldef->colordef, rawcolor);
			out.push_back(t);
		}
	}
	return out;
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
		//										data + (pal_iter + this->paldef.entry_size),
temparr); u32* rawcolor = reinterpret_cast<u32*>(temparr); color t =
this->get_color(*rawcolor); out.push_back(t);
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