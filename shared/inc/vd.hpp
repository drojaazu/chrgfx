#ifndef VD_H
#define VD_H

#include <algorithm>
#include <string>
#include <vector>

namespace vd
{

template <typename T> T vd_int(std::string const &val);

template <typename T> T vd_int_pos(std::string const &val);

template <typename T> T vd_int_nonneg(std::string const &val);

bool vd_bool(std::string const &val);

template <typename T> std::vector<T> vd_int_array(std::string const &val);

} // namespace vd

#endif