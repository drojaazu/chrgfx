#ifndef __MOTOI__SETUP_HPP
#define __MOTOI__SETUP_HPP

#include "shared.hpp"
#include <getopt.h>
#include <iostream>
#include <stdexcept>

using namespace std;

struct runtime_config_palview : runtime_config
{
	string paldata_name;
	string out_path;
	uint pal_line {0};
	bool full_pal {false};
} cfg;

void process_args(int argc, char ** argv)
{
	// add palview specific options
	long_opts.push_back({"pal-data", required_argument, nullptr, 'p'});
	long_opts.push_back({"pal-line", required_argument, nullptr, 'l'});
	long_opts.push_back({"full-pal", no_argument, nullptr, 'f'});
	long_opts.push_back({"output", required_argument, nullptr, 'o'});
	long_opts.push_back({nullptr, 0, nullptr, 0});
	short_opts.append("p:l:fr:o:");

	opt_details.push_back({false, "Path to input encoded palette", nullptr});
	opt_details.push_back({false, "Palette line to render", nullptr});
	opt_details.push_back({false, "Render all palette data instead of a single palette line", nullptr});
	opt_details.push_back({false, "Path to output image", nullptr});

	// read/parse arguments
	while (true)
	{
		const auto this_opt = getopt_long(argc, argv, short_opts.data(), long_opts.data(), nullptr);
		if (this_opt == -1)
			break;

		// handle shared arguments
		if (shared_args(this_opt, cfg))
			continue;

		// handle chr2png specific arguments
		switch (this_opt)
		{
			// input palette data path
			case 'p':
				cfg.paldata_name = optarg;
				break;

				// palette line
			case 'l':
				try
				{
					auto pal_line {stoi(optarg)};
					if (pal_line < 1)
						throw invalid_argument("Invalid palette line value");
					// user input is indexed from 1, convert to 0 index here
					cfg.pal_line = pal_line - 1;
				}
				catch (const invalid_argument & e)
				{
					throw invalid_argument("Invalid palette line value");
				}
				break;

			case 'f':
				cfg.full_pal = true;
				break;

			// png output path
			case 'o':
				cfg.out_path = optarg;
				break;
		}
	}

}

#endif