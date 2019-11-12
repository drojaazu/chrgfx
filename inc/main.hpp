#include <getopt.h>
#include <iostream>
#include <stdio.h>

#include "chrdecode.hpp"
#include "commondefs.hpp"
#include "import_def.hpp"
#include "paldecode.hpp"
#include "render.hpp"

struct runtime_config {
	std::string gfxdef_name{""}, chrdef_name{""}, paldef_name{""};

	std::string chrdata_name{""}, paldata_name{""};

	s16 subpalette;
	render_traits rendertraits;

	std::string outfile;

	runtime_config() { subpalette = -1; }
};

void process_args(int argc, char **argv);
void print_help();
