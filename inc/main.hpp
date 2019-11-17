#ifndef MAIN_H
#define MAIN_H
#include <getopt.h>
#include <iostream>
#include <stdio.h>

#include "chr_conv.hpp"
#include "import_defs.hpp"
#include "pal_conv.hpp"
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

#endif