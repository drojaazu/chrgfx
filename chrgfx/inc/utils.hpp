#ifndef CHRGFX__UTILS_HPP
#define CHRGFX__UTILS_HPP

#include "types.hpp"
#include <algorithm>
#include <png++/png.hpp>

using png::color;
using png::palette;

namespace chrgfx
{
byte_t reduce_bits(byte_t data, u8 bitcount);

byte_t expand_bits(byte_t data, u8 bitcount);

u32 create_bitmask32(u8 bitcount);

u16 create_bitmask16(u8 bitcount);

u8 create_bitmask8(u8 bitcount);

palette make_pal_random();

bool is_system_bigendian();

/**
 * Indicates that the host system uses big endian architecture
 */
extern bool const bigend_sys;
} // namespace chrgfx

#endif