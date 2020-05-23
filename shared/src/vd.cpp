#include "vd.hpp"

namespace vd
{
bool vd_bool(string const &val)
{
	string workval{""};
	std::transform(val.begin(), val.end(), workval.begin(), tolower);
	if(workval == "true" || stoi(workval) > 0) {
		return true;
	}
	if(workval == "false" || stoi(workval) < 1) {
		return false;
	}
	throw "Inavlid value for boolean";
}

} // namespace vd
