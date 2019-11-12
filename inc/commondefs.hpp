#ifndef COMMONDEFS_H
#define COMMONDEFS_H

#include "chrdecode.hpp"
#include "global.hpp"
#include "paldecode.hpp"

namespace chrgfx
{
namespace chrdefs
{
// ----- FALLBACK FORMAT
static chr_def chr_8x8x1 = {
		8,
		8,
		1,
		{0},
		{0, 1, 2, 3, 4, 5, 6, 7},
		{0 * 8, 1 * 8, 2 * 8, 3 * 8, 4 * 8, 5 * 8, 6 * 8, 7 * 8},
		get_chr};

// ----- COMMMON HARDWARE

/**
 * 8x8 4bpp Packed
 * Used by Sega Megadrive, Sharp X68000
 */
static chr_def chr_8x8x4_packed = {
		8,
		8,
		4,
		{0, 1, 2, 3},
		{0, 4, 8, 12, 16, 20, 24, 28},
		{0 * 32, 1 * 32, 2 * 32, 3 * 32, 4 * 32, 5 * 32, 6 * 32, 7 * 32},
		get_chr};

// Used by Nintendo Super Famicom, NEC PC Engine
static chr_def nintendo_sfc = {
		8,
		8,
		4,
		{0, 8, 128, 136},
		{0, 1, 2, 3, 4, 5, 6, 7},
		{0 * 16, 1 * 16, 2 * 16, 3 * 16, 4 * 16, 5 * 16, 6 * 16, 7 * 16},
		get_chr};

// ----- NON-STANDARD CONVERTERS
// TODO: Nintendo SFC 3bpp tiles

} // namespace chrdefs

namespace paldefs
{
// ----- COMMMON HARDWARE
/**
 * Sega Megadrive color format
 * 15|               |0
 *   xxxxBBBx GGGxRRRx
 */
static color_def sega_md_color = {

		1, {1}, {3}, {5}, {3}, {9}, {3}};

/**
 * Sega Megadrive system palette
 * 4 subpalettes of 16 colors each (64 colors total)
 */
static pal_def sega_md_pal = {
		16, 16, 4, &sega_md_color, nullptr, &pal_decode_calc, true};

/**
 * Nintendo Super Famicom color format
 * 15|               |0
 *   xBBBBBGG GGGRRRRR
 * (little endian)
 */
static color_def nintendo_sfc_color = {1, {0}, {5}, {5}, {5}, {10}, {5}};

/**
 * Nintenso Super Famicom system palette
 * 256 colors total
 * Subpalette length varies by video mode
 * (Default is 16 palettes of 16 colors each)
 */
static pal_def nintendo_sfc_pal = {
		16, 16, 16, &nintendo_sfc_color, nullptr, &pal_decode_calc, false};

// ----- NON-STANDARD CONVERTERS
/**
 * TileLayer Pro palette format
 */
static pal_def tilelayerpro = {24,			255,		 1,
															 nullptr, nullptr, &pal_decode_soft_tlp};

} // namespace paldefs

} // namespace chrgfx
#endif