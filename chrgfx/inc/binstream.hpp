#ifndef __MOTOI__BINSTREAM_HPP
#define __MOTOI__BINSTREAM_HPP

#include <cstddef>
#include <iostream>

namespace motoi
{
using iobinstream = std::basic_iostream<std::byte>;

using ibinstream = std::basic_istream<std::byte>;

using obinstream = std::basic_ostream<std::byte>;
} // namespace motoi

#endif
