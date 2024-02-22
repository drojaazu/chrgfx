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

#include <cstdint>
#include <memory>

using uchar = unsigned char;
using uint = unsigned int;
using ushort = unsigned short;
using ulong = unsigned long;

using uint8 = uint8_t;
using uint16 = uint16_t;
using uint32 = uint32_t;

using int8 = int8_t;
using int16 = int16_t;
using int32 = int32_t;

template <typename T>
using uptr = std::unique_ptr<T>;
template <typename T>
using sptr = std::shared_ptr<T>;

#endif
