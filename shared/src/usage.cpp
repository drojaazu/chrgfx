#include "usage.hpp"

/**
 * @brief Displays program options and usage
 *
 * @param opts array of @c option structs; final entry should be all zero
 * @param details array of @c option_details structs; must be the same size and
 * order as @c opts
 * @return std::string
 */
std::string show_usage(option const * opts, option_details const * details)
{
	std::ostringstream oss;

	oss << "Usage:" << std::endl;

	while (true)
	{
		if (opts->name == nullptr)
			break;

		oss << " --" << opts->name << ", -" << (char) opts->val;
		if (opts->has_arg == required_argument)
		{
			if (details->arg_type != nullptr)
				oss << " <" << details->arg_type << ">";
			else
				oss << " <value>";
		}
		if (opts->has_arg == optional_argument)
		{
			if (details->arg_type != nullptr)
				oss << " <optional " << details->arg_type << ">";
			else
				oss << " <optional value>";
		}
		oss << std::endl;
		oss << "    ";
		if (details->required)
			oss << "[Required] ";

		oss << details->desc << std::endl;

		++opts;
		++details;
	}

	return oss.str();
}