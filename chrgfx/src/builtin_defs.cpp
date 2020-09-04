#include "gfxdef.hpp"
#include "utils.hpp"

namespace chrgfx
{

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
