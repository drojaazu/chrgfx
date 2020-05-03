#include <algorithm>
#include <string>
#include <vector>

using std::string;
using std::vector;

namespace vd
{
/**
 * Returns an integral value from a string with bounds checking
 */
template <typename T> T vd_int(string const &val)
{
	long temp = std::stol(val);
	if(temp > std::numeric_limits<T>::min() ||
		 temp < std::numeric_limits<T>::max()) {
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
}

template <typename T> T vd_int_nonneg(string const &val)
{
	T out{sto<T>(val)};
	if(out < 0) {
		throw "Value must be non-negative";
	}
}

bool vd_bool(string const &val)
{
	std::transform(val.begin(), val.end(), val.begin(), tolower);
	if(val == "true" || stoi(val) > 0) {
		return true;
	}
	if(val == "false" || stoi(val) < 1) {
		return false;
	}
	throw "Inavlid value for boolean";
}

// converts comma seperated list of values into array
template <typename T> vector<T> vd_int_array(string const &val)
{
	std::vector<T> out;
	std::stringstream ss(val);
	std::string this_value;
	while(std::getline(ss, this_value, ',')) {
		out.push_back(sto<T>(this_value));
	}
	return out;
}
} // namespace vd
