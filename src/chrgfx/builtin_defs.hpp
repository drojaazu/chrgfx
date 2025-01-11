/**
 * @file builtin_defs.hpp
 * @author Damian Rogers / damian@motoi.pro
 * @copyright ©2022 Motoi Productions / Released under MIT License
 * @brief Internally defined gfxdefs for custom builds, and simple gfxdefs for fallbacks
 */

#ifndef __CHRGFX__BUILTIN_DEFS_HPP
#define __CHRGFX__BUILTIN_DEFS_HPP

#include "chrdef.hpp"
#include "coldef.hpp"
#include "paldef.hpp"
#include <map>

namespace chrgfx::gfxdefs
{

extern chrdef const chr_8x8_1bpp;

extern chrdef const chr_8x8_2bpp_packed_lsb;

extern chrdef const chr_8x8_2bpp_packed_msb;

extern chrdef const chr_8x8_4bpp_packed_lsb;

extern chrdef const chr_8x8_4bpp_packed_msb;

extern chrdef const chr_8x8_8bpp_packed_lsb;

extern chrdef const chr_8x8_8bpp_packed_msb;

extern chrdef const chr_8x8_4bpp_planar;

extern std::map<std::string, chrdef const &> const chrdefs;

extern rgbcoldef const col_bgr_222_packed;

extern rgbcoldef const col_bgr_333_packed;

extern rgbcoldef const col_bgr_444_packed;

extern rgbcoldef const col_bgr_555_packed;

extern std::map<std::string, rgbcoldef const &> const rgbcoldefs;

extern paldef const pal_16bit_8color;

extern paldef const pal_16bit_16color;

extern paldef const pal_16bit_32color;

extern paldef const pal_16bit_64color;

extern paldef const pal_16bit_256color;

extern std::map<std::string, paldef const &> const paldefs;

} // namespace chrgfx::gfxdefs

#endif
