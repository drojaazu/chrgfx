#include "gfxdef.hpp"
#include "utils.hpp"

namespace chrgfx
{

/**
 * This namespace should be used for any built-in gfxdefs.
 * The ones defined by default are very simple and are useful for quickly
 * "previewing" output: for example, if palette data/definition is not yet
 * available but we want to see the tile output.
 *
 * You can add your own built-in definitions here to create a customized library
 * for specific applications.
 */
namespace defs
{

chrdef basic_8x8_1bpp{"basic_8x8_1bpp",
											8,
											8,
											1,
											{1},
											{0, 1, 2, 3, 4, 5, 6, 7},
											{0, 8, 16, 24, 32, 40, 48, 56}};

coldef basic_8bit_random{"basic_8bit_random", make_pal_random()};

paldef basic_256color{"basic_256color", 8, 256, 1};

} // namespace defs
} // namespace chrgfx
