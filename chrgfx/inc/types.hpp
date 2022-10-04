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

//#include <climits>
#include <cstdint>
#include <memory>

using uchar = unsigned char;
using uint = unsigned int;
using ushort = unsigned short;
using ulong = unsigned long;

using u8 = uint8_t;
using u16 = uint16_t;
using u32 = uint32_t;

using s8 = int8_t;
using s16 = int16_t;
using s32 = int32_t;

using byte_t = uint8_t;

template <typename T>
using uptr = std::unique_ptr<T>;
template <typename T>
using sptr = std::shared_ptr<T>;

#endif
