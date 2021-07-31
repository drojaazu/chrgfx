#ifndef CHRGFX__COLDEF_H
#define CHRGFX__COLDEF_H

#include <png++/png.hpp>
#include <string>
#include <vector>

#include "gfxdef.hpp"
#include "rgb_layout.hpp"

using png::color;
using png::palette;
using std::string;
using std::vector;

namespace chrgfx
{
	/**
	 * Specifying a table of RGB values that approximately correspond to the
	 * output of the hardware (reftab based)
	 */
	class refcoldef : public gfxdef
	{
	protected:
		palette reftab;
		const bool is_big_endian;

	public:
		refcoldef(string id, palette reftab, bool is_big_endian = false) :
				gfxdef(std::move(id)), reftab(std::move(reftab)),
				is_big_endian(is_big_endian) {};

		/**
		 * Returns the color from the reference palette for the given index
		 */
		color getReftabEntry(ushort index) const
		{
			return reftab[index];
		}

		/**
		 * Returns the index to the color matching the RGB value provided, or its
		 * nearest color
		 */
		ushort getReftabIndex(color rgb) const
		{
			size_t idx { 0 };
			for(auto & this_color : reftab)
			{
				if(this_color.red == rgb.red && this_color.green == rgb.green &&
					 this_color.blue == rgb.blue)
				{
					return idx;
				}
				++idx;
			}

			// this could certainly use some tuning, but it mostly works
			std::vector<std::pair<int, int>> distances;
			distances.reserve(this->reftab.size());
			int pal_color_iter { 0 };
			for(const auto & this_color : this->reftab)
			{
				int this_distance = (abs(this_color.red - rgb.red)) +
														(abs(this_color.green - rgb.green)) +
														(abs(this_color.blue - rgb.blue));
				distances.push_back(std::pair<int, int>(pal_color_iter, this_distance));
				++pal_color_iter;
			}

			int dist_check { distances[0].second };
			idx = 0;
			for(const auto & this_entry : distances)
			{
				if(std::get<1>(this_entry) < dist_check)
				{
					dist_check = this_entry.second;
					idx = this_entry.first;
				}
			}

			return idx;
		}

		/**
		 * \brief Returns true if the original harware is big endian (reftab based)
		 */
		bool getIsBigEndian() const
		{
			return is_big_endian;
		}
	};

	/**
	 * Defines color data in one of two ways:
	 *   - Specifying offsets and width of each RGB color channel within the
	 * data (rgblayout based)

	 */
	class rgbcoldef : public gfxdef
	{
	protected:
		vector<rgb_layout> layout;
		ushort bitdepth;
		const bool is_big_endian;

	public:
		/**
		 * Constructor for an rgblayout based coldef
		 */
		rgbcoldef(string id, ushort bitdepth, vector<rgb_layout> layout,
							bool is_big_endian = false) :
				gfxdef(std::move(id)),
				bitdepth(bitdepth), layout(std::move(layout)),
				is_big_endian(is_big_endian) {};

		/**
		 * Constructor for a reftab based coldef
		 */

		/**chrdef
		 * Returns true if this coldef is reftab based
		 */
		bool useReftab() const;

		/**
		 * Returns the vector of RGB layouts (rgblayout based)
		 */
		vector<rgb_layout> get_rgb_layout() const;

		/**
		 * Returns the RGB bit layout for the given pass (rgblayout based)
		 */
		rgb_layout get_rgb_pass(ushort pass) const
		{
			return layout[pass];
		}

		/**
		 * Returns the bitdepth of the color channels
		 */
		ushort getBitdepth() const
		{
			return bitdepth;
		}

		/**
		 * Returns true if the original harware is big endian (reftab based)
		 */
		bool get_is_big_endian() const
		{
			return is_big_endian;
		}
	};
} // namespace chrgfx

#endif