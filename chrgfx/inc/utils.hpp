#ifndef CHRGFX__UTILS_HPP
#define CHRGFX__UTILS_HPP

#include "basic_gfx.hpp"
#include "types.hpp"

namespace chrgfx
{
/**
 * @brief Reduce the bitdepth of an 8 bit color channel
 *
 * @param data color channel data
 * @param bitcount target bitdepth
 */
uint8 reduce_bitdepth(uint8 data, uint8 bitdepth);

/**
 * @brief Increase bitdepth of a color channel to 8 bits
 *
 * @param data color channel data
 * @param bitdepth bitdepth of the input channel
 */
uint8 expand_bitdepth(uint8 data, uint8 bitdepth);

/**
 * @brief Returns an 32 bit value with the specified number of bits set
 * starting from the LSB
 */
uint32 create_bitmask32(uint8 bitcount);

/**
 * @brief Returns an 16 bit value with the specified number of bits set
 * starting from the LSB
 */
uint16 create_bitmask16(uint8 bitcount);

/**
 * @brief Returns an 8 bit value with the specified number of bits set
 * starting from the LSB
 */
uint8 create_bitmask8(uint8 bitcount);

/**
 * @brief Create an 8 bit palette of randomized colors
 */
basic_palette make_pal_random();

/**
 * @brief Determines the endianness of the local system
 * @note This is called on initialization of libchrgfx; use the @c
 * bigend_sys variable to quickly check endianness
 */
bool is_system_bigendian();

/**
 * @brief Indicates that the local system uses big endian architecture
 */
extern bool const bigend_sys;
} // namespace chrgfx

#endif