#include "basic_gfx.hpp"

#ifndef __CHRGFX__TILESETCONV_HPP
#define __CHRGFX__TILESETCONV_HPP

namespace chrgfx
{

basic_image render_tileset(chrdef const & chrdef, motoi::blob<basic_pixel> const & chrdata, render_config const & rcfg);

motoi::blob<byte_t> segment_tileset(chrdef const & chrdef, basic_image const & bitmap);

} // namespace chrgfx

#endif
