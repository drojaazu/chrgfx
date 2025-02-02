#include "image.hpp"

/**
	These are convenience aliases to re-contextualize these types with terms that more commonly appear in applications
	likely to be used by chrgfx
 */

namespace chrgfx
{

using rgb_color = motoi::pixel_type::rgb;

using pixel = motoi::pixel_type::index;

using palette = motoi::color_map_8bpp;

using image = motoi::image<pixel>;

} // namespace chrgfx
