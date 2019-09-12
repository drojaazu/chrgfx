#ifndef COMMONDEFS_H
#define COMMONDEFS_H
#include "paldecode.hpp"

namespace chrgfx
{
namespace chrdefs
{
/*
u16 width;												// pixel width of each element
u16 height;												// pixel height of each element
u16 planes;												// number of bitplanes
u32 planeoffset[MAX_GFX_PLANES];	// bit offset of each bitplane
u32 xoffset[MAX_GFX_SIZE];				// bit offset of each horizontal pixel
u32 yoffset[MAX_GFX_SIZE];				// bit offset of each vertical pixel
u32 charincrement;	// distance between two consecutive elements (in bits)
 */

const static chr_def chr_8x8x1 = {
		8,
		8,
		1,
		{0},
		{0, 1, 2, 3, 4, 5, 6, 7},
		{0 * 8, 1 * 8, 2 * 8, 3 * 8, 4 * 8, 5 * 8, 6 * 8, 7 * 8},
		8 * 8};

// used by Sega Megadrive, X68000
const static chr_def chr_8x8x4_packed = {
		8,
		8,
		4,
		{0, 1, 2, 3},
		{0, 4, 8, 12, 16, 20, 24, 28},
		{0 * 32, 1 * 32, 2 * 32, 3 * 32, 4 * 32, 5 * 32, 6 * 32, 7 * 32},
		8 * 32};

// used by Sega Game Gear, Sega Mark III/Master System, Bandai Wonderswan
const static chr_def sega_8bit = {
		8,
		8,
		4,
		{24, 16, 8, 0},
		{0, 1, 2, 3, 4, 5, 6, 7},
		{0 * 32, 1 * 32, 2 * 32, 3 * 32, 4 * 32, 5 * 32, 6 * 32, 7 * 32},
		8 * 32};

// Used by Nintendo Super Famicom, NEC PC Engine
const static chr_def nintendo_sfc = {
		8,
		8,
		4,
		{0, 8, 128, 136},
		{0, 1, 2, 3, 4, 5, 6, 7},
		{0 * 16, 1 * 16, 2 * 16, 3 * 16, 4 * 16, 5 * 16, 6 * 16, 7 * 16},
		8 * 32};

// Used by Super Famicom, Gameboy, Gameboy Color
const static chr_def nintendo_2bpp = {
		8,
		8,
		2,
		{8, 0},
		{0, 1, 2, 3, 4, 5, 6, 7},
		//{7, 6, 5, 4, 3, 2, 1, 0},
		{0 * 16, 1 * 16, 2 * 16, 3 * 16, 4 * 16, 5 * 16, 6 * 16, 7 * 16},
		8 * 16};

const static chr_def nintendo_fc = {
		8,
		8,
		2,
		{64, 0},
		{0, 1, 2, 3, 4, 5, 6, 7},
		{0 * 8, 1 * 8, 2 * 8, 3 * 8, 4 * 8, 5 * 8, 6 * 8, 7 * 8},
		8 * 16};

const chr_def seta_chr = {
		16,
		16,
		4,
		{0, 4, 8, 12},
		//{387, 386, 385, 384, 259, 258, 257, 256, 131, 130, 129, 128, 3, 2, 1, 0},
		{384, 385, 386, 387, 256, 257, 258, 259, 128, 129, 130, 131, 0, 1, 2, 3},
		{0, 16, 32, 48, 64, 80, 96, 112, 512, 528, 544, 560, 576, 592, 608, 624},
		16 * 16 * 4};

const chr_def seta_6bit_chr = {
		16,
		16,
		6,
		{0, 4, 8, 12, 16, 20},
		{579, 578, 577, 576, 387, 386, 385, 384, 195, 194, 193, 192, 3, 2, 1, 0},
		{0, 24, 48, 72, 96, 120, 144, 168, 768, 792, 816, 840, 864, 888, 912, 936},
		16 * 16 * 6};

const chr_def seta_8bit_chr = {
		16,
		16,
		8,
		{0, 4, 8, 12, 16, 20, 24, 28},
		{771, 770, 769, 768, 515, 514, 513, 512, 259, 258, 257, 256, 3, 2, 1, 0},
		{0, 32, 64, 96, 128, 160, 192, 224, 1024, 1056, 1088, 1120, 1152, 1184,
		 1216, 1248},
		16 * 16 * 8};
}	// namespace chrdefs

namespace paldefs
{
const static color_def sega_md_color = {
		// 15|               |0
		//   xxxxBBBx GGGxRRRx
		1,
		{1},
		{7},
		{5},
		{7},
		{9},
		{7},
		endianness::big

};

const static pal_def sega_md_pal = {
		16, 16, 4, &pal_decode_calc, &sega_md_color, nullptr};

const static palette nintendo_gb_classic_syspal = {
		color(0xc4, 0xcf, 0xa1),	// 00 - lightest
		color(0x8b, 0x95, 0x6d),	// 01
		color(0x6b, 0x73, 0x53),	// 02
		color(0x41, 0x41, 0x41)		// 03 - darkest
};

// 4 colors, 2 bits per color, packed (1 byte per palette)
// 3 palettes (3 bytes, stored at 0xFF47 in memory)
const static pal_def nintendo_gb_classic_pal = {
		2, 4, 3, &pal_decode_fixed, nullptr, &nintendo_gb_classic_syspal};

// const static pal_def nintendo_gb_pal = {32, 4, 1, nintendo_gb_classic_pal,
//																				nullptr};

}	// namespace paldefs
}	// namespace chrgfx
#endif