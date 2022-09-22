/**
 * @file usage.cpp
 * @author Motoi Productions (Damian Rogers damian@motoi.pro)
 * @brief Display program options/usage
 *
 * Updates:
 * 20220415 Initial
 * 20220722 Using const in show_usage parameters
 */

#include "usage.hpp"
#include "app.hpp"

void show_version(std::wostream & output)
{
	std::wstringstream ss;
	ss << APP::NAME << " - version " << APP::VERSION << std::endl;
	ss << APP::CONTACT << " / " << APP::WEBSITE << std::endl;

	output << ss.str();
}

void show_usage(
	option const * opts, option_details const * details, std::wostream & output)
{
	setlocale(LC_ALL, "");

	show_version(output);

	output << std::endl << "Usage:" << std::endl;

	while(true)
	{
		if(opts->name == nullptr)
			break;

		output << " --" << opts->name << ", -" << (char) opts->val;
		if(opts->has_arg == required_argument)
		{
			if(details->arg_type != nullptr)
				output << " <" << details->arg_type << ">";
			else
				output << " <value>";
		}
		if(opts->has_arg == optional_argument)
		{
			if(details->arg_type != nullptr)
				output << " <optional " << details->arg_type << ">";
			else
				output << " <optional value>";
		}
		output << std::endl;
		output << "    ";
		if(details->required)
			output << "[Required] ";

		output << details->desc << std::endl;

		++opts;
		++details;
	}
}
