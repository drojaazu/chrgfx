#ifndef RENDER_H
#define RENDER_H
#include "types.hpp"

namespace chrgfx
{
png::image<png::index_pixel> *render(bank &chr_bank, png::palette &pal,
																		 render_traits &traits);
}
#endif

/*
there should be two conversion functions per data type
one take the def only, and will always use the default conversion method
ther other takes a def and a function pointer to the conversion method

in the case of palettes, for default, it will check if there is a syspal
or coldef, and will call the respective method
*/