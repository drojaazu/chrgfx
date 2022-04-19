/**
 * @file usage.hpp
 * @author Motoi Productions (Damian Rogers damian@motoi.pro)
 * @brief Display program options/usage
 *
 * Updates:
 * 20220415 Initial
 */

#include "usage.hpp"
#include "app.hpp"
#include <sstream>

using namespace std;

void show_usage(option * opts, option_details * details, std::wostream & output)
{
	setlocale(LC_ALL, "");

	wstringstream ss;
	ss << APP::NAME << " - version " << APP::VERSION << endl;
	ss << APP::CONTACT << " / " << APP::WEBSITE << endl << endl;
	ss << "Usage:" << endl;

	while(1)
	{
		if(opts->name == 0)
			break;

		ss << " --" << opts->name << ", -" << (char)opts->val;
		if(opts->has_arg == required_argument)
		{
			if(details->arg_type != nullptr)
				ss << " <" << details->arg_type << ">";
			else
				ss << " <value>";
		}
		if(opts->has_arg == optional_argument)
		{
			if(details->arg_type != nullptr)
				ss << " <optional " << details->arg_type << ">";
			else
				ss << " <optional value>";
		}
		ss << endl;
		ss << "    ";
		if(details->required)
			ss << "[Required] ";

		ss << details->desc << endl;

		++opts;
		++details;
	}

	output << ss.str();
}
