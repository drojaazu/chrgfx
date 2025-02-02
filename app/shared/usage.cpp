/**
 * @file usage.cpp
 * @author Motoi Productions (Damian Rogers damian@motoi.pro)
 * See accompanying header file for additional information
 */

#include "usage.hpp"
#include "app.hpp"
#include <getopt.h>
#include <ostream>

namespace motoi
{

using namespace std;

void show_usage(option const * opts, option_details const * details, ostream & os)
{
	setlocale(LC_ALL, "");

	os << APP::app_info();

	os << "Usage:" << '\n';

	while (true)
	{
		if (opts->name == nullptr)
			break;

		if (! iscntrl(opts->val))
			os << " -" << (char) opts->val << ",";
		os << " --" << opts->name;
		if (opts->has_arg == required_argument)
		{
			if (details->arg_type != nullptr)
				os << "=" << details->arg_type;
			else
				os << "=value";
		}
		if (opts->has_arg == optional_argument)
		{
			if (details->arg_type != nullptr)
				os << "[=" << details->arg_type << "]";
			else
				os << "=[value]";
		}
		os << '\n';
		os << "    ";
		if (details->required)
			os << "[Required] ";

		os << details->desc << '\n';

		++opts;
		++details;
	}
}
} // namespace motoi
