#include "imgfmt_png.hpp"

using namespace std;

namespace chrgfx
{

image from_png(png::image<png::index_pixel> const & png_image)
{
	auto png_pixbuf = png_image.get_pixbuf();
	auto height {png_pixbuf.get_height()}, width {png_pixbuf.get_width()};
	image basic_img(width, height);
	byte_t * ptr = basic_img.pixel_map();
	for (uint i_row {0}; i_row < height; ++i_row)
	{
		byte_t const * s = (byte_t const *) (png_pixbuf.get_row(i_row).data());
		copy(s, s + width, ptr);
		ptr += width;
	}

	auto png_palette = png_image.get_palette();
	palette basic_pal;
	size_t counter {0};
	for (auto const & color : png_palette)
		basic_pal[counter++] = rgb_color(color.red, color.green, color.blue);

	basic_img.color_map(basic_pal);

	return basic_img;
}

png::image<png::index_pixel> to_png(image const & basic_image, optional<uint8> trns_index)
{
	if (basic_image.color_map()->size() < 256)
		throw invalid_argument("Palette must contain a full 256 entries for PNG export");

	png::pixel_buffer<png::index_pixel> png_pixbuf(basic_image.width(), basic_image.height());
	for (uint i_pixel_row {0}; i_pixel_row < basic_image.height(); ++i_pixel_row)
	{
		auto * ptr = (png::index_pixel *) (basic_image.pixel_map() + i_pixel_row * basic_image.width());
		vector<png::index_pixel> pxlrow_work(ptr, ptr + basic_image.width());
		png_pixbuf.put_row(i_pixel_row, pxlrow_work);
	}
	png::image<png::index_pixel> outimg(basic_image.width(), basic_image.height());
	outimg.set_pixbuf(png_pixbuf);

	vector<png::color> png_pal;
	for (auto const & color : *basic_image.color_map())
		png_pal.emplace_back(color.red, color.green, color.blue);
	outimg.set_palette(png_pal);

	// setup transparency
	if (trns_index)
	{
		png::tRNS trans(256, 255);
		trans[trns_index.value()] = 0;
		outimg.set_tRNS(trans);
	}

	return outimg;
}

} // namespace chrgfx