#ifndef __MOTOI__SETUP_HPP
#define __MOTOI__SETUP_HPP

#include "shared.hpp"
#include <string>

struct runtime_config_png2chr : runtime_config
{
	std::string pngdata_path;
	std::string out_chrdata_path;
	std::string out_paldata_path;
} cfg;

void process_args(int argc, char ** argv)
{
	// add png2chr specific options
	long_opts.push_back({"chr-output", required_argument, nullptr, 'c'});
	long_opts.push_back({"pal-output", required_argument, nullptr, 'p'});
	long_opts.push_back({"png-data", required_argument, nullptr, 'b'});
	long_opts.push_back({nullptr, 0, nullptr, 0});
	short_opts.append("c:p:b:");

	opt_details.push_back({true, "Path to output encoded tiles", nullptr});
	opt_details.push_back({true, "Path to output encoded palette", nullptr});
	opt_details.push_back({true, "Path to input PNG image", nullptr});

	// read/parse arguments
	while (true)
	{
		const auto this_opt = getopt_long(argc, argv, short_opts.data(), long_opts.data(), nullptr);
		if (this_opt == -1)
			break;

		// handle shared arguments
		if (shared_args(this_opt, cfg))
			continue;

		// handle png2chr specific arguments
		switch (this_opt)
		{
			// chr-output
			case 'c':
				cfg.out_chrdata_path = optarg;
				break;

			// pal-output
			case 'p':
				cfg.out_paldata_path = optarg;
				break;

			// png-data
			case 'b':
				cfg.pngdata_path = optarg;
				break;
		}
	}
}

#endif