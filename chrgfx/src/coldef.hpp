#ifndef CHRGFX__COLDEF_H
#define CHRGFX__COLDEF_H

#include <png++/png.hpp>
#include <string>
#include <vector>

#include "gfxdef.hpp"
#include "rgb_layout.hpp"

namespace chrgfx
{
	using namespace std;
	using namespace png;

	/**
	 * Specifying a table of RGB values that approximately correspond to the
	 * output of the hardware (reftab based)
	 */
	class refcoldef : public gfxdef
	{
	protected:
		palette p_reftab;
		const bool p_big_endian;

	public:
		refcoldef(string id, palette reftab, bool is_big_endian = false) :
				gfxdef(std::move(id)), p_reftab(std::move(reftab)),
				p_big_endian(is_big_endian) {};

		/**
		 * Returns the color from the reference palette for the given index
		 */
		color reftabColor(ushort index) const;

		/**
		 * Returns the index to the color matching the RGB value provided, or its
		 * nearest color
		 */
		ushort reftabIndex(color rgb) const;

		/**
		 * \brief Returns true if the original harware is big endian (reftab based)
		 */
		bool bigEndian() const;
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
		 * Returns the vector of RGB layouts (rgblayout based)
		 */
		vector<rgb_layout> & rgbLayout();

		/**
		 * Returns the RGB bit layout for the given pass (rgblayout based)
		 */
		rgb_layout get_rgb_pass(ushort pass) const;

		/**
		 * Returns the bitdepth of the color channels
		 */
		ushort getBitdepth() const;

		/**
		 * Returns true if the original harware is big endian (reftab based)
		 */
		bool get_is_big_endian() const;
	};
} // namespace chrgfx

#endif