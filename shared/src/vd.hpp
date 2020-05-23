#ifndef VD_H
#define VD_H

#include "shared.hpp"
#include <algorithm>
#include <istream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

using std::string;
using std::vector;

namespace vd
{

template <typename T> T sto(string const &val)
{
	long temp = std::stol(val);
	if(temp < std::numeric_limits<T>::min() ||
		 temp > std::numeric_limits<T>::max()) {
		throw std::out_of_range("Invalid string value");
	}
	return (T)temp;
}

template <typename T> T vd_int_pos(string const &val)
{
	T out{sto<T>(val)};
	if(out <= 0) {
		throw "Value must be greater than zero";
	}
	return out;
}

template <typename T> T vd_int_nonneg(string const &val)
{
	T out{sto<T>(val)};
	if(out < 0) {
		throw "Value must be non-negative";
	}
	return out;
}

// converts comma seperated list of values into array
template <typename T> vector<T> vd_int_array(string const &val)
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