/**
 * @file types.hpp
 * @author Motoi Productions (Damian Rogers damian@motoi.pro)
 * @brief Commonly used typedefs
 *
 * Updates:
 * 20200101 Initial
 */

#ifndef __MOTOI__TYPES_HPP
#define __MOTOI__TYPES_HPP

#include <memory>
#include <stdint.h>

typedef unsigned char uchar;
typedef unsigned int uint;
typedef unsigned short ushort;
typedef unsigned long ulong;

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;

typedef int8_t s8;
typedef int16_t s16;
typedef int32_t s32;

typedef char byte_t;

template <typename T> using uptr = std::unique_ptr<T[]>;
template <typename T> using sptr = std::shared_ptr<T[]>;

#endif
