#ifndef CHRGFX__BUILTIN_DEFS_H
#define CHRGFX__BUILTIN_DEFS_H

#include "chrdef.hpp"
#include "coldef.hpp"
#include "paldef.hpp"
#include "utils.hpp"

namespace chrgfx
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
	namespace defs
	{

		/**
		 * Minimal, generic CHR format
		 * 1bpp 8x8
		 */
		chrdef basic_8x8_1bpp { "basic_8x8_1bpp",
														8,
														8,
														1,
														{ 1 },
														{ 0, 1, 2, 3, 4, 5, 6, 7 },
														{ 0, 8, 16, 24, 32, 40, 48, 56 } };

		/**
		 * Generates random 8bit RGB values
		 */
		refcoldef basic_8bit_random { "basic_8bit_random", make_pal_random() };

		/**
		 * Generic 8bit (256 color) palette
		 */
		paldef basic_256color { "basic_256color", 8, 256, 1 };

	} // namespace defs
} // namespace chrgfx

#endif