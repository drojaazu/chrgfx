#include "pngpal.hpp"

namespace chrgfx
{
u8 *pngpal(paldef const &paldef, coldef const &coldef,
							 png::palette &palette)
{
	return to_pal(paldef, coldef, palette, 0);
}

} // namespace chrgfx