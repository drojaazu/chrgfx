#ifndef __CHRGFX__CUSTOM_HPP
#define __CHRGFX__CUSTOM_HPP

#include "types.hpp"
#include <png++/png.hpp>

namespace chrgfx
{
namespace custom
{

byte_t * decode_chr_nintendo_sfc_3bpp(byte_t const * encoded_chr, byte_t * out);

png::palette decode_pal_tilelayerpro(std::istream & tpl_palette);

} // namespace custom
} // namespace chrgfx

#endif
