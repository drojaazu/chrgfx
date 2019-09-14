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

const chr_def capcom_cps1 = {
		16,
		16,
		4,
		{24, 16, 8, 0},
		{0, 1, 2, 3, 4, 5, 6, 7, 32, 33, 34, 35, 36, 37, 38, 39},
		{0, 64, 128, 192, 256, 320, 384, 448, 512, 576, 640, 704, 768, 832, 896,
		 960},
		4 * 16 * 16};

const chr_def snk_neogeo = {
		16,
		16,
		4,
		{24, 16, 8, 0},
		{519, 518, 517, 516, 515, 514, 513, 512, 7, 6, 5, 4, 3, 2, 1, 0},
		{0, 32, 64, 96, 128, 160, 192, 224, 256, 288, 320, 352, 384, 416, 448, 480},
		4 * 16 * 16};

const chr_def snk_neogeocd = {
		16,
		16,
		4,
		{16, 24, 0, 8},
		{519, 518, 517, 516, 515, 514, 513, 512, 7, 6, 5, 4, 3, 2, 1, 0},
		{0, 32, 64, 96, 128, 160, 192, 224, 256, 288, 320, 352, 384, 416, 448, 480},
		4 * 16 * 16};

const chr_def capcom_cps3 = {
		16,
		16,
		8,
		{0, 1, 2, 3, 4, 5, 6, 7},
		{3 * 8, 2 * 8, 1 * 8, 0 * 8, 7 * 8, 6 * 8, 5 * 8, 4 * 8, 11 * 8, 10 * 8,
		 9 * 8, 8 * 8, 15 * 8, 14 * 8, 13 * 8, 12 * 8},
		{0, 128, 256, 384, 512, 640, 768, 896, 1024, 1152, 1280, 1408, 1536, 1664,
		 1792, 1920},
		8 * 256};

const chr_def saturn_16x16x8 = {
		16,
		16,
		8,
		{0, 1, 2, 3, 4, 5, 6, 7},
		{0, 8, 16, 24, 32, 40, 48, 56, 64 * 8 + 0, 65 * 8, 66 * 8, 67 * 8, 68 * 8,
		 69 * 8, 70 * 8, 71 * 8

		},
		{0 * 64, 1 * 64, 2 * 64, 3 * 64, 4 * 64, 5 * 64, 6 * 64, 7 * 64, 64 * 16,
		 64 * 17, 64 * 18, 64 * 19, 64 * 20, 64 * 21, 64 * 22, 64 * 23},
		128 * 16 /* really 128*16, but granularity is 32 bytes */
};
const chr_def saturn_16x16x4 = {
		16,
		16,
		4,
		{0, 1, 2, 3},
		{
				0,
				4,
				8,
				12,
				16,
				20,
				24,
				28,
				32 * 8 + 0,
				32 * 8 + 4,
				32 * 8 + 8,
				32 * 8 + 12,
				32 * 8 + 16,
				32 * 8 + 20,
				32 * 8 + 24,
				32 * 8 + 28,

		},
		{0 * 32, 1 * 32, 2 * 32, 3 * 32, 4 * 32, 5 * 32, 6 * 32, 7 * 32, 32 * 16,
		 32 * 17, 32 * 18, 32 * 19, 32 * 20, 32 * 21, 32 * 22, 32 * 23

		},
		32 * 32};

const static chr_def saturn_8x8x8 = {
		8,
		8,
		8,
		{0, 1, 2, 3, 4, 5, 6, 7},
		{0, 8, 16, 24, 32, 40, 48, 56},
		{0 * 64, 1 * 64, 2 * 64, 3 * 64, 4 * 64, 5 * 64, 6 * 64, 7 * 64},
		64 * 8 /* really 64*8, but granularity is 32 bytes */
};
}	// namespace chrdefs

namespace paldefs
{
const static color_def sega_md_color = {
		// 15|               |0
		//   xxxxBBBx GGGxRRRx
		1, {1}, {3}, {5}, {3}, {9}, {3}};

const static color_def snk_neogeo_color = {
		3, {15, 14, 8}, {1, 1, 4}, {15, 13, 4}, {1, 1, 4}, {15, 12, 0}, {1, 1, 4}};

const static color_def snk_neogeo_noshadow_color = {
		2, {14, 8}, {1, 4}, {13, 4}, {1, 4}, {12, 0}, {1, 4}};

const static pal_def snk_neogeo_pal = {&pal_decode_calc,	16,			16, 16,
																			 &snk_neogeo_color, nullptr};

const static pal_def snk_neogeo_noshadow_pal = {&pal_decode_calc, 16, 16, 16,
																								&snk_neogeo_noshadow_color};

const static pal_def sega_md_pal = {
		&pal_decode_calc, 16, 16, 4, &sega_md_color, nullptr, endianness::big};

const static palette nintendo_gb_classic_syspal = {
		color(0xc4, 0xcf, 0xa1),	// 00 - lightest
		color(0x8b, 0x95, 0x6d),	// 01
		color(0x6b, 0x73, 0x53),	// 02
		color(0x41, 0x41, 0x41)		// 03 - darkest
};

// a 'standard' 2C02 PPU palette, from:
// https://wiki.nesdev.com/w/index.php/PPU_palettes

const static palette nintendo_fc_syspal = {
		color(0x54, 0x54, 0x54),	// 0x00
		color(0x00, 0x1E, 0x74), color(0x08, 0x10, 0x2C), color(0x30, 0x00, 0x88),
		color(0x44, 0x00, 0x64), color(0x5C, 0x00, 0x30), color(0x54, 0x04, 0x00),
		color(0x3C, 0x18, 0x00), color(0x20, 0x2A, 0x00), color(0x08, 0x3A, 0x00),
		color(0x00, 0x40, 0x00), color(0x00, 0x3C, 0x00), color(0x00, 0x32, 0x3C),
		color(0x00, 0x00, 0x00), color(0x00, 0x00, 0x00), color(0x00, 0x00, 0x00),
		color(0x98, 0x96, 0x98),	// 0x10
		color(0x08, 0x4C, 0xC4), color(0x30, 0x32, 0xEC), color(0x5C, 0x1E, 0xE4),
		color(0x88, 0x14, 0xB0), color(0xA0, 0x14, 0x64), color(0x98, 0x22, 0x20),
		color(0x78, 0x3C, 0x00), color(0x54, 0x5A, 0x00), color(0x28, 0x72, 0x00),
		color(0x08, 0x7C, 0x00), color(0x00, 0x76, 0x28), color(0x00, 0x66, 0x78),
		color(0x00, 0x00, 0x00), color(0x00, 0x00, 0x00), color(0x00, 0x00, 0x00),
		color(0xEC, 0xEE, 0xEC),	// 0x20
		color(0x4C, 0x9A, 0xEC), color(0x78, 0x7C, 0xEC), color(0xB0, 0x62, 0xEC),
		color(0xE4, 0x54, 0xEC), color(0xEC, 0x58, 0xB4), color(0xEC, 0x6A, 0x64),
		color(0xD4, 0x88, 0x20), color(0xA0, 0xAA, 0x00), color(0x74, 0xC4, 0x00),
		color(0x4C, 0xD0, 0x20), color(0x38, 0xCC, 0x6C), color(0x38, 0xB4, 0xCC),
		color(0x3C, 0x3C, 0x3C), color(0x00, 0x00, 0x00), color(0x00, 0x00, 0x00),
		color(0xEC, 0xEE, 0xEC),	// 0x30
		color(0xA8, 0xCC, 0xEC), color(0xBC, 0xBC, 0xEC), color(0xD4, 0xB2, 0xEC),
		color(0xEC, 0xAE, 0xEC), color(0xEC, 0xAE, 0xD4), color(0xEC, 0xB4, 0xB0),
		color(0xE4, 0xC4, 0x90), color(0xCC, 0xD2, 0x78), color(0xB4, 0xDE, 0x78),
		color(0xA8, 0xE2, 0x90), color(0x98, 0xE2, 0xB4), color(0xA0, 0xD6, 0xE4),
		color(0xA0, 0xA2, 0xA0), color(0x00, 0x00, 0x00), color(0x00, 0x00, 0x00)};

// 2 bits per color, 4 colors, packed (1 byte per palette)
// 3 palettes (3 bytes, stored at 0xFF47 in memory)
// TODO: check endianness
const static pal_def nintendo_gb_classic_pal = {
		&pal_decode_fixed, 2, 4, 3, nullptr, &nintendo_gb_classic_syspal};

// 32 colors, each color is 1 byte; 8 palettes, 4 colors each (color 0 is always
// transparent)
const static pal_def nintendo_fc_pal = {
		&pal_decode_fixed, 8, 4, 8, nullptr, &nintendo_fc_syspal,
		endianness::little};

const static pal_def tilelayerpro = {&pal_decode_soft_tlp};
}	// namespace paldefs

}	// namespace chrgfx
#endif