/**
 * @file usage.cpp
 * @author Motoi Productions (Damian Rogers damian@motoi.pro)
 * @brief Display program options/usage
 *
 * Updates:
 * 20220415 Initial
 * 20220722 Using const in show_usage parameters
 * 20220914 Added show_version
 */

#include "usage.hpp"
#include "app.hpp"

void show_version (std::wostream & output)
{
	std::wstringstream ss;
	ss << APP::NAME << " - version " << APP::VERSION << std::endl;
	ss << APP::CONTACT << " / " << APP::WEBSITE << std::endl;

	output << ss.str ();
}

void show_usage (option const * opts, option_details const * details, std::wostream & output)
{
	setlocale (LC_ALL, "");

	std::wstringstream ss;
	ss << APP::NAME << " - version " << APP::VERSION << std::endl;
	ss << APP::CONTACT << " / " << APP::WEBSITE << std::endl << std::endl;
	ss << "Usage:" << std::endl;

	while (true)
	{
		if (opts->name == nullptr)
			break;

		ss << " --" << opts->name << ", -" << (char) opts->val;
		if (opts->has_arg == required_argument)
		{
			if (details->arg_type != nullptr)
				ss << " <" << details->arg_type << ">";
			else
				ss << " <value>";
		}
		if (opts->has_arg == optional_argument)
		{
			if (details->arg_type != nullptr)
				ss << " <optional " << details->arg_type << ">";
			else
				ss << " <optional value>";
		}
		ss << std::endl;
		ss << "    ";
		if (details->required)
			ss << "[Required] ";

		ss << details->desc << std::endl;

		++opts;
		++details;
	}

	output << ss.str ();
}
