#ifndef CHRGFX__GFXDEF_H
#define CHRGFX__GFXDEF_H

#include "types.hpp"
#include <optional>
#include <png++/png.hpp>
#include <string>
#include <vector>

using png::color;
using png::palette;
using std::pair;
using std::string;
using std::vector;

namespace chrgfx
{
	/**
	 * Defines the bit positions of RGB channels
	 * Positive shift values shift right; negative values shift left
	 */
	class rgb_layout
	{
	public:
		rgb_layout(pair<int, uint> red, pair<int, uint> green,
							 pair<int, uint> blue);

		int get_red_shift() const;
		uint get_red_size() const;
		int get_green_shift() const;
		uint get_green_size() const;
		int get_blue_shift() const;
		uint get_blue_size() const;

	private:
		pair<int, uint> red;
		pair<int, uint> green;
		pair<int, uint> blue;
	};

	/**
	 * Abtract class for a graphics definition
	 */
	class gfxdef
	{
		/**
		 * Returns the id of this definition
		 */
	public:
		string get_id() const;

	protected:
		gfxdef(string const & id);

	private:
		string id;
	};

	/**
	 * Defines the pixel layout of a graphics tile
	 */
	class chrdef : public gfxdef
	{
	public:
		chrdef(string id, uint width, uint height, uint bitplanes,
					 vector<uint> planeoffset, vector<uint> pixeloffset,
					 vector<uint> rowoffset,
					 converters::cvto_chr_t converter_to = converters::cvto_chr,
					 converters::cvfrom_chr_t converter_from = converters::cvfrom_chr);

		/**
		 * Returns the width of the tile, in pixels
		 */
		uint get_width() const;

		/**
		 * Returns the height of the tile, in pixels
		 */
		uint getHeight() const;

		/**
		 * Returns the bit depth of the tile
		 */
		uint getBitdepth() const;

		/**
		 * Returns the offset to a given bitplane in a row, in bits
		 */
		uint get_planeoffset_at(uint pos) const;

		/**
		 * Returns the offset to a given pixel in a row, in bits
		 */
		uint get_pixeloffset_at(uint pos) const;

		/**
		 * Returns the offset to a given row in the tile, in bits
		 */
		uint get_rowoffset_at(uint pos) const;

		/**
		 * Returns the size of a single tile, in bits
		 */
		uint get_datasize() const;

		converters::cvfrom_chr_t get_converter_from() const;

		converters::cvto_chr_t get_converter_to() const;

	private:
		uint width;
		uint height;
		uint bitdepth;

		vector<uint> planeoffset;
		uint const * planeoffset_data;

		vector<uint> pixeloffset;
		uint const * pixeloffset_data;

		vector<uint> rowoffset;
		uint const * rowoffset_data;

		uint datasize; // size of one chr in bits

		converters::cvfrom_chr_t converter_from;
		converters::cvto_chr_t converter_to;
	};

	/**
	 * Defines color data in one of two ways:
	 *   - Specifying offsets and width of each RGB color channel within the data
	 *     (rgblayout based)
	 *   - Specifying a table of RGB values that approximately correspond to the
	 *     output of the hardware (reftab based)
	 */
	class rgbcoldef : public gfxdef
	{
	public:
		/**
		 * Constructor for an rgblayout based coldef
		 */
		rgbcoldef(
				string id, uint bitdepth, vector<rgb_layout> layout,
				bool is_big_endian = false,
				converters::cvto_col_t converter_to = converters::convertToColor,
				converters::cvfrom_col_t converter_from = converters::convertFromColor);

		/**
		 * Constructor for a reftab based coldef
		 */
		rgbcoldef(
				string id, palette reftab, bool is_big_endian = false,
				converters::cvto_col_t converter_to = converters::convertToColor,
				converters::cvfrom_col_t converter_from = converters::convertFromColor);

		/**
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
		rgb_layout get_rgb_pass(uint pass) const;

		/**
		 * Returns the bitdepth of the color channels
		 */
		uint get_bitdepth() const;

		/**
		 * Returns the color from the reference palette for the given index (reftab
		 * based)
		 */
		color get_reftab_entry(uint index) const;

		/**
		 * Returns the index to the color matching the RGB value provided, or its
		 * nearest color (reftab based)
		 */
		uint getReftabIndex(color rgb) const;

		/**
		 * Returns true if the original harware is big endian (reftab based)
		 */
		bool get_is_big_endian() const;

		converters::cvfrom_col_t get_converter_from() const;

		converters::cvto_col_t get_converter_to() const;

	private:
		vector<rgb_layout> layout;
		palette reftab;
		const bool is_reftab;
		uint bitdepth;
		const bool is_big_endian;

		converters::cvfrom_col_t converter_from;
		converters::cvto_col_t converter_to;
	};

	/**
	 * Defines the structure of a system palette
	 */
	class paldef : public gfxdef
	{
	public:
		paldef(
				string id, uint entry_datasize, uint subpal_length, uint subpal_count,
				std::optional<uint> subpal_datasize = std::nullopt,
				conv_palette::cvto_pal_t converter_to = conv_palette::cvto_pal,
				conv_palette::cvfrom_pal_t converter_from = conv_palette::cvfrom_pal);

		/**
		 * Returns the size in bits of a single color entry
		 * (shouldn't this be in coldef....?)
		 */
		uint get_entry_datasize() const;

		/**
		 * Returns the number of entries in the full palette
		 */
		uint get_palette_length() const;

		/**
		 * Returns the number of entries in a subpalette
		 */
		uint get_subpal_length() const;

		/**
		 * Returns the number of subpalettes in the full palette
		 */
		uint get_subpal_count() const;

		/**
		 * Returns the data size of the full palette, in bits
		 */
		uint get_palette_datasize() const;

		/**
		 * Returns the data size of the full palette, in bytes
		 */
		uint get_palette_datasize_bytes() const;

		/**
		 * Returns the data size of a subpalette (in bits)
		 */
		uint get_subpal_datasize() const;

		/**
		 * Returns the data size of a subpalette (in bytes)
		 */
		uint get_subpal_datasize_bytes() const;

		conv_palette::cvto_pal_t get_converter_to() const;

		conv_palette::cvfrom_pal_t get_converter_from() const;

	private:
		uint entryDatasize;
		uint subpalSize;
		uint subpalCount;
		uint subpalDatasize;

		conv_palette::cvto_pal_t converter_to;
		conv_palette::cvfrom_pal_t converter_from;
	};

} // namespace chrgfx

#endif
