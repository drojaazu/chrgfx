#include "imaging.hpp"
#include "shared.hpp"
#include <getopt.h>
#include <stdexcept>
#include <string>

struct runtime_config_chr2png : runtime_config
{
	std::string chrdata_path;
	std::string paldata_path;
	chrgfx::render_config render_cfg;
	std::string out_png_path;
	uint pal_line {0};
} cfg;

void process_args(int argc, char ** argv)
{
	// add chr2png specific options
	long_opts.push_back({"chr-data", required_argument, nullptr, 'c'});
	long_opts.push_back({"pal-data", required_argument, nullptr, 'p'});
	long_opts.push_back({"pal-line", required_argument, nullptr, 'l'});
	long_opts.push_back({"trns-index", required_argument, nullptr, 'i'});
	long_opts.push_back({"row-size", required_argument, nullptr, 'r'});
	long_opts.push_back({"output", required_argument, nullptr, 'o'});
	long_opts.push_back({nullptr, 0, nullptr, 0});
	short_opts.append("c:p:l:i:r:o:");

	opt_details.push_back({false, "Path to input encoded tiles", nullptr});
	opt_details.push_back({false, "Path to input encoded palette", nullptr});
	opt_details.push_back({false, "Palette line to use for PNG output", nullptr});
	opt_details.push_back({false, "Palette index to use for transparency", nullptr});
	opt_details.push_back({false, "Number of tiles per row in output image", nullptr});
	opt_details.push_back({false, "Path to output PNG image", nullptr});

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
			// input tile data path
			case 'c':
				cfg.chrdata_path = optarg;
				break;

			// input palette data path
			case 'p':
				cfg.paldata_path = optarg;
				break;

			// palette line
			case 'l':
				try
				{
					auto pal_line {std::stoi(optarg)};
					if (pal_line < 1)
						throw std::invalid_argument("Invalid palette line value");
					// user input is indexed from 1, convert to 0 index here
					cfg.pal_line = pal_line - 1;
				}
				catch (const std::invalid_argument & e)
				{
					throw std::invalid_argument("Invalid palette line value");
				}
				break;

			// palette entry index for transparency
			case 'i':
				try
				{
					std::string s = optarg;
					auto trns_index {stoi(s)};
					if (trns_index < 0)
						throw std::invalid_argument("Invalid transparency index value");
					cfg.render_cfg.trns_index = trns_index;
				}
				catch (const std::invalid_argument & e)
				{
					throw std::invalid_argument("Invalid transparency index value");
				}
				break;

			// row size
			case 'r':
				try
				{
					cfg.render_cfg.row_size = std::stoi(optarg);
				}
				catch (const std::invalid_argument & e)
				{
					throw std::invalid_argument("Invalid columns value");
				}
				break;

			// png output path
			case 'o':
				cfg.out_png_path = optarg;
				break;
		}
	}
}