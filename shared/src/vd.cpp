#include "vd.hpp"

namespace vd
{
using namespace std;

bool vd_bool(string const & val)
{
	string workval { val };
	transform(workval.begin(), workval.end(), workval.begin(),
						[](unsigned char c) { return tolower(c); });
	if(workval == "true" || stoi(workval) > 0)
	{
		return true;
	}
	if(workval == "false" || stoi(workval) < 1)
	{
		return false;
	}
	throw std::invalid_argument("Inavlid boolean value");
}

} // namespace vd
