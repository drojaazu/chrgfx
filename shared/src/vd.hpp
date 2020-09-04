#ifndef VD_H
#define VD_H

#include "shared.hpp"
#include <algorithm>
#include <istream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

namespace vd
{

template <typename T> T sto(std::string const &val, int base = 10)
{
	signed long temp{std::stol(val, nullptr, base)};
	if(temp < std::numeric_limits<T>::min() ||
		 temp > std::numeric_limits<T>::max()) {
		throw std::out_of_range("Value does not fit in specified type");
	}
	return (T)temp;
}

template <typename T> T vd_int_pos(std::string const &val)
{
	T out{sto<T>(val)};
	if(out <= 0) {
		throw std::out_of_range("Value must be greater than zero");
	}
	return out;
}

template <typename T> T vd_int_nonneg(std::string const &val)
{
	T out{sto<T>(val)};
	if(out < 0) {
		throw std::out_of_range("Value must be non-negative");
	}
	return out;
}

/**
 * Converts a comma delimited list of integer values into a vector
 */
template <typename T> std::vector<T> vd_int_array(std::string const &val)
{
	std::vector<T> out;

	std::istringstream ss{val};
	std::string this_value;

	while(std::getline(ss, this_value, ',')) {
		out.push_back(sto<T>(this_value));
	}
	return out;
}

bool vd_bool(std::string const &val);

} // namespace vd

#endif