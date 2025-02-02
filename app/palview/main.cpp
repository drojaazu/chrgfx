#include "blob.hpp"
#include "filesys.hpp"
#include "gfxdefman.hpp"
#include "image.hpp"
#include "imageformat_png.hpp"
#include "setup.hpp"
#include <chrgfx/chrgfx.hpp>

#include <iostream>
#include <png++/rgb_pixel.hpp>

using namespace std;
using namespace chrgfx;
using namespace motoi;

// uint const swatch_size {32};

chrgfx::coldef const * work_coldef {&chrgfx::gfxdefs::col_bgr_333_packed};
chrgfx::paldef const * work_paldef {&chrgfx::gfxdefs::pal_16bit_256color};

int main(int argc, char ** argv)
{
	try
	{

		process_args(argc, argv);

		gfxdef_manager defs(cfg);
		work_coldef = defs.coldef();
		work_paldef = defs.paldef();
		ifstream is_paldata {ifstream_checked(cfg.paldata_name)};

		if (cfg.full_pal)
		{
			blob paldata {is_paldata};
			auto image = render_palette_full(*work_paldef, *work_coldef, paldata, paldata.size());
			png::image<png::rgb_pixel> outimg(image.width(), image.height());
			for (auto i {0}; i < image.height(); ++i)
			{
				auto p = (png::basic_rgb_pixel<byte_t> *) image.pixel_map_row(i);
				auto c = vector<png::basic_rgb_pixel<byte_t>>(p, p + image.width());
				outimg.get_pixbuf().put_row(i, c);
			}

			if (cfg.out_path.empty())
				outimg.write_stream(cout);
			else
				outimg.write(cfg.out_path);
		}
		else
		{
			size_t pal_size {work_paldef->datasize_bytes()};
			auto palbuffer {unique_ptr<byte_t>(new byte_t[pal_size])};
			is_paldata.seekg(cfg.pal_line * pal_size, ios::beg);
			is_paldata.read(reinterpret_cast<char *>(palbuffer.get()), pal_size);
			if (! is_paldata.good())
				throw runtime_error("Cannot read specified palette line index");

			auto image = render_palette(*work_paldef, *work_coldef, palbuffer.get());
			auto outimg {to_png(image)};
			if (cfg.out_path.empty())
				outimg.write_stream(cout);
			else
				outimg.write(cfg.out_path);
		}

		return 0;
	}
	catch (exception const & e)
	{
		cerr << "Error: " << e.what() << '\n';
		return -1;
	}
}
