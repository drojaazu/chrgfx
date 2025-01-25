/**
 * @file utils.hpp
 * @author Damian Rogers / damian@motoi.pro
 * @copyright ©2022 Motoi Productions / Released under MIT License
 * @brief General purpose utilities
 */

#ifndef __CHRGFX__UTILS_HPP
#define __CHRGFX__UTILS_HPP

#include "image_types.hpp"
#include "types.hpp"

namespace chrgfx
{

// clang-format off
// borrowed from MAME
#define STEP2(START,STEP)       (START),(START)+(STEP)
#define STEP4(START,STEP)       STEP2(START,STEP),STEP2((START)+2*(STEP),STEP)
#define STEP8(START,STEP)       STEP4(START,STEP),STEP4((START)+4*(STEP),STEP)
#define STEP16(START,STEP)      STEP8(START,STEP),STEP8((START)+8*(STEP),STEP)
#define STEP32(START,STEP)      STEP16(START,STEP),STEP16((START)+16*(STEP),STEP)
#define STEP64(START,STEP)      STEP32(START,STEP),STEP32((START)+32*(STEP),STEP)
#define STEP128(START,STEP)     STEP64(START,STEP),STEP64((START)+64*(STEP),STEP)
#define STEP256(START,STEP)     STEP128(START,STEP),STEP128((START)+128*(STEP),STEP)
#define STEP512(START,STEP)     STEP256(START,STEP),STEP256((START)+256*(STEP),STEP)
#define STEP1024(START,STEP)    STEP512(START,STEP),STEP512((START)+512*(STEP),STEP)
#define STEP2048(START,STEP)    STEP1024(START,STEP),STEP1024((START)+1024*(STEP),STEP)

#define STEP2_INV(START,STEP)   (START)+(STEP),(START)
#define STEP4_INV(START,STEP)    STEP2_INV(START+2*STEP,STEP),STEP2_INV(START,STEP)
// clang-format on

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
palette make_pal_random();

/**
 * @brief Determines the endianness of the local system
 * @note This is called on initialization of libchrgfx; use the @c
 * bigend_sys variable to quickly check endianness
 */
bool is_system_big_endian();

/**
 * @brief Indicates that the local system uses big endian architecture
 */
extern bool const bigend_sys;
} // namespace chrgfx

#endif