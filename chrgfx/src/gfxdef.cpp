#include "types.hpp"

namespace chrgfx
{

class gfx_def
{
public:
	const std::string &get_id() { return id; };

protected:
	gfx_def(std::string const &id) : id(id){};

private:
	std::string id;
};

/**
 * Describes a graphic tile by specifying dimensions and bitplane and pixel
 * offsets
 */
class chr_def : public gfx_def
{
public:
	chr_def(std::string const &id, u16 const width, u16 const height,
					u8 const bitplanes, std::vector<u32> const &planeoffset,
					std::vector<u32> const &pixeloffset,
					std::vector<u32> const &rowoffset)
			: gfx_def(std::move(id)), width(std::move(width)),
				height(std::move(height)), bitplanes(std::move(bitplanes)),
				planeoffset(std::move(planeoffset)), pixeloffset(pixeloffset),
				rowoffset(std::move(rowoffset)),
				datasize(std::move(width * height * bitplanes)),
				planeoffset_data(std::move(this->planeoffset.data())),
				pixeloffset_data(std::move(this->pixeloffset.data())),
				rowoffset_data(std::move(this->rowoffset.data())){};

	u16 get_width() { return width; }
	u16 get_height() { return height; }
	u8 get_bitplanes() { return bitplanes; }

	u32 *get_planeoffset() { return planeoffset_data; };
	u32 *get_pixeloffset() { return pixeloffset_data; };
	u32 *get_rowoffset() { return rowoffset_data; };
	u32 get_pixeloffset_at(size_t pos) { return pixeloffset_data[pos]; };
	u32 get_rowoffset_at(size_t pos) { return rowoffset_data[pos]; };
	u32 get_planeoffset_at(size_t pos) { return planeoffset_data[pos]; };

	u32 get_datasize() { return datasize; }

private:
	u16 width;
	u16 height;
	u8 bitplanes;

	std::vector<u32> planeoffset;
	u32 *planeoffset_data;

	std::vector<u32> pixeloffset;
	u32 *pixeloffset_data;

	std::vector<u32> rowoffset;
	u32 *rowoffset_data;

	u32 datasize; // size of one chr in bits
};

/**
 * Describes color data by specifying the offset and data size of each component
 * (red, green, blue) within the data
 */
class col_def : public gfx_def
{
public:
	col_def(const std::string id, const std::vector<rgb_layout> layout,
					bool is_big_endian = false)
			: gfx_def(std::move(id)), layout(std::move(layout)),
				is_big_endian(std::move(is_big_endian)), is_refpal(false){};

	col_def(const std::string id, const palette &refpal,
					bool is_big_endian = false)
			: gfx_def(std::move(id)), refpal(std::move(refpal)),
				is_big_endian(std::move(is_big_endian)), is_refpal(true){};

	rgb_layout get_rgb_pass(size_t pass) { return layout.at(pass); }

	std::vector<rgb_layout> get_rgb_layout() { return layout; }

	color get_pal_idx(size_t index) { return refpal.at(index); }

	bool use_refpal() { return is_refpal; }
	bool get_is_big_endian() { return is_big_endian; }

private:
	std::vector<rgb_layout> layout;
	palette refpal;
	const bool is_refpal;

	/**
	 * Specify the endianness of the color data
	 */
	const bool is_big_endian;
};

/**
 * Defines the format of a color palette by specifying the size and count of
 * subpalettes, as well as either a color definition or pre-defined color set
 */
class pal_def : public gfx_def
{
public:
	pal_def();

	pal_def(std::string const &id, u8 const entry_datasize,
					u16 const subpal_length, u16 const subpal_count,
					bool const is_big_endian = false, u8 const subpal_datasize = 0)
			: gfx_def(std::move(id)), entry_datasize(std::move(entry_datasize)),
				subpal_length(std::move(subpal_length)),
				subpal_count(std::move(subpal_count)),

				subpal_datasize(std::move(subpal_datasize)){};

	u8 get_entry_datasize() { return entry_datasize; }

	u16 get_subpal_length() { return subpal_length; }

	u16 get_subpal_count() { return subpal_count; }

	u8 get_subpal_datasize() { return subpal_datasize; }

private:
	/**
	 * The size of each color entry in bits
	 */
	u8 entry_datasize;

	/**
	 * The number entries in a subpalette
	 */
	u16 subpal_length;

	/**
	 * The number of subpalettes in a full palette
	 */
	u16 subpal_count;

	u8 subpal_datasize;
};

class rgb_layout
{

public:
	rgb_layout(std::pair<s8, u8> red, std::pair<s8, u8> green,
						 std::pair<s8, u8> blue)
			: red(red), green(green), blue(blue){};

	s8 get_red_shift() { return red.first; }
	u8 get_red_count() { return red.second; }
	s8 get_green_shift() { return green.first; }
	u8 get_green_count() { return green.second; }
	s8 get_blue_shift() { return blue.first; }
	u8 get_blue_count() { return blue.second; }

private:
	std::pair<s8, u8> red;
	std::pair<s8, u8> green;
	std::pair<s8, u8> blue;
};

} // namespace chrgfx