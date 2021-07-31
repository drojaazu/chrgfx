#ifndef CHRGFX__UTILS_H
#define CHRGFX__UTILS_H

#include "types.hpp"
#include <algorithm>
#include <png++/png.hpp>

using png::color;
using png::palette;

namespace chrgfx
{
palette make_pal_random();

/**
 * Indicates that the host system uses big endian architecture
 */
extern bool const bigend_sys;
} // namespace chrgfx

#endif