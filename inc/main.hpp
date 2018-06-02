#include <getopt.h>
#include <stdio.h>
#include <iostream>
#include <map>

#include "nintendo_sfc_gfx.hpp"
#include "render.hpp"
#include "sega_md_gfx.hpp"
#include "tlp_palette.hpp"
/*
enum chr_xformers
{
	sega_md_cx,
	nintendo_sfc_cx
};

enum pal_xformers
{
	sega_md_px,
	nintendo_sfc_px,
	tilelayerpro_px
};
*/
void process_args(int argc, char** argv);
void print_help();
