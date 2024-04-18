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

namespace chrgfx::gfxdefs
{

/**
 * This namespace should be used for any gfxdefs to be built in to the final
 * binary
 *
 * The ones defined by default are very simple and are useful for quickly
 * "previewing" output: for example, if palette data/definition is not yet
 * available but we want to see the tile output.
 *
 * You can add your own built-in definitions here to create a customized
 * library for specific applications.
 */

/**
 * @brief Minimal, generic CHR format (1bpp 8x8)
 */
extern chrdef basic_8x8_1bpp;

/**
 * @brief Randomized 8 bit color palette
 */
extern refcoldef basic_8bit_random;

/**
 * @brief Generic 8 bit (256 color) palette
 */
extern paldef basic_256color;

} // namespace chrgfx::gfxdefs

#endif
